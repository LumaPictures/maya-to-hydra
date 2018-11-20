//
// Copyright 2018 Luma Pictures
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
#include <hdmaya/adapters/dagAdapter.h>

#include <hdmaya/adapters/adapterDebugCodes.h>
#include <hdmaya/adapters/mayaAttrs.h>

#include <pxr/base/tf/type.h>
#include <pxr/imaging/hd/tokens.h>

#include <maya/MDagMessage.h>
#include <maya/MFnDagNode.h>
#include <maya/MNodeMessage.h>
#include <maya/MPlug.h>

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfType) {
    TfType::Define<HdMayaDagAdapter, TfType::Bases<HdMayaAdapter> >();
}

namespace {

void _TransformNodeDirty(MObject& node, MPlug& plug, void* clientData) {
    auto* data = reinterpret_cast<HdMayaDagAdapter::CallbackData*>(clientData);
    TF_DEBUG(HDMAYA_ADAPTER_DAG_PLUG_DIRTY)
        .Msg(
            "Dag adapter marking prim (%s) dirty because %s plug was "
            "dirtied.\n",
            data->adapter->GetID().GetText(), plug.partialName().asChar());
    if (plug == MayaAttrs::dagNode::visibility) {
        if (data->adapter->UpdateVisibility()) {
            // Transform can change while dag path is hidden.
            data->adapter->MarkDirty(
                HdChangeTracker::DirtyVisibility |
                HdChangeTracker::DirtyTransform);
            data->adapter->InvalidateTransform();
        }
    } else if (data->adapter->IsVisible()) {
        data->adapter->MarkDirty(HdChangeTracker::DirtyTransform);
        data->adapter->InvalidateTransform();
        data->valid = false;
        data->matrix.SetIdentity();
    }
}

void _HierarchyChanged(MDagPath& child, MDagPath& parent, void* clientData) {
    TF_UNUSED(child);
    TF_UNUSED(parent);
    auto* adapter = reinterpret_cast<HdMayaDagAdapter*>(clientData);
    adapter->GetDelegate()->RecreateAdapter(
        adapter->GetID(), adapter->GetNode());
}

} // namespace

HdMayaDagAdapter::HdMayaDagAdapter(
    const SdfPath& id, HdMayaDelegateCtx* delegate, const MDagPath& dagPath)
    : HdMayaAdapter(dagPath.node(), id, delegate), _dagPath(dagPath) {
    UpdateVisibility();
}

void HdMayaDagAdapter::_CalculateTransform() {
    if (_invalidTransform) {
        if (_callbackData.empty()) {
            _transform = GetGfMatrixFromMaya(_dagPath.inclusiveMatrix());
        } else {
            _transform.SetIdentity();
            for (auto& data: _callbackData) {
                if (!data.valid) {
                    MStatus status;
                    MFnDagNode transform(data.path, &status);
                    if (status) {
                        data.matrix = GetGfMatrixFromMaya(
                            transform.transformationMatrix());
                    }
                    data.valid = true;
                }
                _transform *= data.matrix;
            }
        }
        _invalidTransform = false;
    }
};

const GfMatrix4d& HdMayaDagAdapter::GetTransform() {
    TF_DEBUG(HDMAYA_ADAPTER_GET)
        .Msg(
            "Called HdMayaDagAdapter::GetTransform() - %s\n",
            _dagPath.partialPathName().asChar());
    _CalculateTransform();
    return _transform;
}

void HdMayaDagAdapter::CreateCallbacks() {
    MStatus status;
    auto dag = GetDagPath();
    if (dag.node() != dag.transform()) { dag.pop(); }
    HdMayaAdapter::CreateCallbacks();
    const auto length = dag.length();
    if (length == 0) {
        _callbackData.clear();
        return;
    }
    _callbackData.resize(dag.length());
    for (auto i = decltype(length){0}; i < length; ++i, dag.pop()) {
        MObject obj = dag.node();
        if (obj != MObject::kNullObj) {
            auto& data = _callbackData[i];
            data.adapter = this;
            data.path = dag;
            data.matrix.SetIdentity();
            data.valid = false;
            auto id = MNodeMessage::addNodeDirtyPlugCallback(
                obj, _TransformNodeDirty, _callbackData.data() + i, &status);
            if (status) { AddCallback(id); }
            _AddHierarchyChangedCallback(dag);
        }
    }

}

void HdMayaDagAdapter::MarkDirty(HdDirtyBits dirtyBits) {
    if (dirtyBits != 0) {
        GetDelegate()->GetChangeTracker().MarkRprimDirty(GetID(), dirtyBits);
    }
}

void HdMayaDagAdapter::RemovePrim() { GetDelegate()->RemoveRprim(GetID()); }

void HdMayaDagAdapter::PopulateSelection(
    const HdSelection::HighlightMode& mode, HdSelection* selection) {
    selection->AddRprim(mode, GetID());
}

bool HdMayaDagAdapter::UpdateVisibility() {
    if (ARCH_UNLIKELY(!GetDagPath().isValid())) { return false; }
    const auto visible = GetDagPath().isVisible();
    if (visible != _isVisible) {
        _isVisible = visible;
        return true;
    }
    return false;
}

void HdMayaDagAdapter::_AddHierarchyChangedCallback(MDagPath& dag) {
    MStatus status;
    auto id = MDagMessage::addParentAddedDagPathCallback(
        dag, _HierarchyChanged, this, &status);
    if (status) { AddCallback(id); }
}

void HdMayaDagAdapter::RemoveCallbacks() {
    _callbackData.clear();
    HdMayaAdapter::RemoveCallbacks();
}

PXR_NAMESPACE_CLOSE_SCOPE
