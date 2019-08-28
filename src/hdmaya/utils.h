//
// Copyright 2019 Luma Pictures
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http:#www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#ifndef __HDMAYA_UTILS_H__
#define __HDMAYA_UTILS_H__

#include <pxr/pxr.h>

#include <hdmaya/api.h>

#include <pxr/base/gf/matrix4d.h>
#include <pxr/base/tf/token.h>

#include <pxr/imaging/hd/textureResource.h>

#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MItDag.h>
#include <maya/MItSelectionList.h>
#include <maya/MMatrix.h>
#include <maya/MPlug.h>
#include <maya/MRenderUtil.h>
#include <maya/MSelectionList.h>

#include <hdmaya/adapters/mayaAttrs.h>

#include <functional>
#include <tuple>

PXR_NAMESPACE_OPEN_SCOPE

inline GfMatrix4d GetGfMatrixFromMaya(const MMatrix& mayaMat) {
    GfMatrix4d mat;
    memcpy(mat.GetArray(), mayaMat[0], sizeof(double) * 16);
    return mat;
}

/// \brief Returns the texture file path from a "file" shader node.
HDMAYA_API
TfToken GetFileTexturePath(const MFnDependencyNode& fileNode);

/// \brief Returns the texture resource from a "file" shader node.
HDMAYA_API
HdTextureResourceSharedPtr GetFileTextureResource(
    const MObject& fileObj, const TfToken& filePath,
    int maxTextureMemory = 4 * 1024 * 1024);

/// \brief Returns the texture wrapping parameters from a "file" shader node.
HDMAYA_API
std::tuple<HdWrap, HdWrap> GetFileTextureWrappingParams(const MObject& fileObj);

/// \brief Runs a function on all recursive descendents of a selection list
///        May optionally filter by node type. The items in the list
///        are also included in the set of items that are iterated over
///        (assuming they pass the filter).
template <typename FUNC>
inline void MapSelectionDescendents(
    const MSelectionList& sel, FUNC func,
    MFn::Type filterType = MFn::kInvalid) {
    MStatus status;
    MItDag itDag;
    MDagPath currentSelDag;
    MDagPath currentDescendentDag;
    for (MItSelectionList itSel(sel); !itSel.isDone(); itSel.next()) {
        if (itSel.itemType() != MItSelectionList::kDagSelectionItem) {
            continue;
        }
        if (!itSel.getDagPath(currentSelDag)) {
            // our check against itemType means that we should always
            // succeed in getting the dag path, so warn if we don't
            TF_WARN("Error getting dag path from selection");
            continue;
        }

        // We make sure that no parent of the selected item is
        // also selected - otherwise, we would end up re-traversing the
        // same subtree
        bool parentSelected = false;
        MDagPath parentDag = currentSelDag;
        parentDag.pop();
        for (; parentDag.length() > 0; parentDag.pop()) {
            if (sel.hasItem(parentDag)) {
                parentSelected = true;
                break;
            }
        }
        if (parentSelected) { continue; }

        // Now we iterate through all dag descendents of the current
        // selected item
        itDag.reset(currentSelDag, MItDag::kDepthFirst, filterType);
        for (; !itDag.isDone(); itDag.next()) {
            status = itDag.getPath(currentDescendentDag);
            if (!status) {
                // just to print an error
                CHECK_MSTATUS(status);
                continue;
            }
            func(currentDescendentDag);
        }
    }
}

PXR_NAMESPACE_CLOSE_SCOPE

#endif // __HDMAYA_UTILS_H_
