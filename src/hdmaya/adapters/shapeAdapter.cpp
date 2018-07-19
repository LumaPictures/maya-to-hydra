#include <hdmaya/adapters/shapeAdapter.h>

#include <pxr/base/tf/type.h>

#include <maya/MPlug.h>
#include <maya/MPlugArray.h>

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<HdMayaShapeAdapter, TfType::Bases<HdMayaDagAdapter> >();
}

HdMayaShapeAdapter::HdMayaShapeAdapter(const SdfPath& id, HdMayaDelegateCtx* delegate, const MDagPath& dagPath)
    : HdMayaDagAdapter(id, delegate, dagPath) {
    _CalculateExtent();
}

void
HdMayaShapeAdapter::_CalculateExtent() {
    MStatus status;
    MFnDagNode dagNode(GetDagPath(), &status);
    if (ARCH_LIKELY(status)) {
        const auto bb = dagNode.boundingBox();
        const auto mn = bb.min();
        const auto mx = bb.max();
        _extent.SetMin({mn.x, mn.y, mn.z});
        _extent.SetMax({mx.x, mx.y, mx.z});
    }
};

HdMeshTopology
HdMayaShapeAdapter::GetMeshTopology() {
    return {};
};

HdPrimvarDescriptorVector
HdMayaShapeAdapter::GetPrimvarDescriptors(HdInterpolation interpolation) {
    return {};
}

void
HdMayaShapeAdapter::MarkDirty(HdDirtyBits dirtyBits) {
    HdMayaDagAdapter::MarkDirty(dirtyBits);
    if (dirtyBits & HdChangeTracker::DirtyPoints) {
        _CalculateExtent();
    }
}

MObject
HdMayaShapeAdapter::GetMaterial() {
    MStatus status;
    MFnDagNode dagNode(GetDagPath(), &status);
    if (!status) { return MObject::kNullObj; }

    auto instObjGroups = dagNode.findPlug("instObjGroups");
    if (instObjGroups.isNull()) { return MObject::kNullObj; }

    MPlugArray conns;
    // TODO: deal with instancing properly.
    instObjGroups.elementByLogicalIndex(0).connectedTo(conns, false, true);

    const auto numConnections = conns.length();
    if (numConnections == 0) { return MObject::kNullObj; }
    for (auto i = decltype(numConnections){0}; i < numConnections; ++i) {
        auto sg = conns[i].node();
        if (sg.apiType() == MFn::kShadingEngine) {
            return sg;
        }
    }

    return MObject::kNullObj;
}

PXR_NAMESPACE_CLOSE_SCOPE