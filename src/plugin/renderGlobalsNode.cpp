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
#include "renderGlobalsNode.h"

#include <pxr/imaging/hdx/rendererPluginRegistry.h>

#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnNumericAttribute.h>

#include "renderOverride.h"
#include "utils.h"

PXR_NAMESPACE_OPEN_SCOPE

namespace {

MObject _rendererPlugins;

std::unordered_map<TfToken, HdRenderSettingDescriptorList, TfToken::HashFunctor>
    _rendererAttributes;
} // namespace

const MString MtohRenderGlobalsNode::name("MtohRenderGlobals");
// Part of the internal Luma node id block
const MTypeId MtohRenderGlobalsNode::typeId(0x00116EFC);

MStatus MtohRenderGlobalsNode::Initialize() {
    MFnEnumAttribute eAttr;
    _rendererPlugins = eAttr.create("rendererPlugins", "rp");

    {
        const auto rendererPlugins = MtohGetRendererPlugins();
        short rendererIndex = 0;
        for (const auto& rendererPlugin : rendererPlugins) {
            eAttr.addField(rendererPlugin.GetText(), rendererIndex);
            rendererIndex += 1;
        }
    }
    eAttr.setDefault(MtohGetDefaultRenderer().GetText());

    addAttribute(_rendererPlugins);

    return MS::kSuccess;
}

void MtohRenderGlobalsNode::postConstructor() {
    setExistWithoutInConnections(true);
    setExistWithoutOutConnections(true);
}

void MtohRenderGlobalsNode::ReadRenderDelegateAttributes() {
    for (const auto& rendererPluginName : MtohGetRendererPlugins()) {
        auto* rendererPlugin =
            HdxRendererPluginRegistry::GetInstance().GetRendererPlugin(
                rendererPluginName);
        if (rendererPlugin == nullptr) { continue; }
        auto* renderDelegate = rendererPlugin->CreateRenderDelegate();
        if (renderDelegate == nullptr) { continue; }
        _rendererAttributes[rendererPluginName] =
            renderDelegate->GetRenderSettingDescriptors();
        delete renderDelegate;
    }
}

PXR_NAMESPACE_CLOSE_SCOPE
