// Copyright eeGeo Ltd (2012-2019), All Rights Reserved

#include "OfflineRoutingGraphPositioner.h"
#include "IOfflineRoutingDataRepository.h"
#include "OfflineRoutingPointOnGraph.h"
#include "OfflineRoutingGraphNode.h"
#include "OfflineRoutingFeature.h"
#include "InteriorId.h"
#include "SingleSphere.h"

#include <limits>

namespace ExampleApp
{
    namespace OfflineRouting
    {
        namespace SdkModel
        {
            namespace RoutingEngine
            {
                OfflineRoutingGraphPositioner::OfflineRoutingGraphPositioner(const IOfflineRoutingDataRepository& offlineRoutingDataRepository)
                : m_offlineRoutingDataRepository(offlineRoutingDataRepository)
                {}

                OfflineRoutingPointOnGraph OfflineRoutingGraphPositioner::FindPointOnGraph(const Eegeo::dv3& point,
                                                                                           const Eegeo::Resources::Interiors::InteriorId& interiorId,
                                                                                           const int floorId,
                                                                                           const double maxDistance)
                {
                    if (!interiorId.IsValid())
                    {
                        return OfflineRoutingPointOnGraph();
                    }

                    double bestDistance = std::numeric_limits<double>::max();
                    float bestParameterisedSplineDistance = 0.f;
                    Eegeo::dv3 bestPointOnSpline = Eegeo::dv3::Zero();
                    OfflineRoutingFeatureId bestFeatureId = 0;

                    const Eegeo::Geometry::SingleSphere sphere = { point.ToSingle(), static_cast<float>(maxDistance) };

                    const OfflineRoutingFeatures& features = m_offlineRoutingDataRepository.GetFeatures();

                    for (const auto& featureIt : features)
                    {
                        const auto& feature = featureIt.second;
                        if (feature.GetIsMultiFloor())
                        {
                            continue;
                        }

                        if (feature.GetInteriorId() != interiorId)
                        {
                            continue;
                        }

                        const auto& featureNodes = feature.GetLinkedNodes();
                        const OfflineRoutingGraphNode& featureNode = m_offlineRoutingDataRepository.GetGraphNode(featureNodes.front());

                        if (featureNode.GetFloorId() != floorId)
                        {
                            continue;
                        }

                        const auto& featureSpline = feature.GetSpline();

                        Eegeo_ASSERT(featureNodes.size() == featureSpline.Points().size());

                        const auto& featureSplineBounds = featureSpline.Bounds();

                        if (!featureSplineBounds.intersectsSphere(sphere))
                        {
                            continue;
                        }

                        float t = 0.f;
                        const float distanceToPoint = featureSpline.DistanceToPoint(point.ToSingle(), t);
                        if (distanceToPoint >= maxDistance)
                        {
                            continue;
                        }

                        if (distanceToPoint >= bestDistance)
                        {
                            continue;
                        }

                        bestDistance = distanceToPoint;
                        bestParameterisedSplineDistance = t;
                        bestFeatureId = feature.GetId();
                        bestPointOnSpline = featureSpline.GetInterpolatedPoint(t);
                    }

                    if (bestFeatureId == 0)
                    {
                        return OfflineRoutingPointOnGraph();
                    }

                    const auto& closestFeature = m_offlineRoutingDataRepository.GetFeature(bestFeatureId);
                    const auto& nodes = closestFeature.GetLinkedNodes();

                    std::vector<OfflineRoutingGraphNodeId> nodeEdges;
                    bool isPositionedOnNode = false;

                    for (const auto& nodeId : nodes)
                    {
                        const auto& node = m_offlineRoutingDataRepository.GetGraphNode(nodeId);
                        if (bestPointOnSpline.SquareDistanceTo(node.GetPoint()) < MinimumDistanceInMeters)
                        {
                            isPositionedOnNode = true;
                            nodeEdges.push_back(nodeId);
                            break;
                        }
                    }

                    if (!isPositionedOnNode)
                    {
                        int nodeIndex = closestFeature.GetSpline().IndexImmediatelyBefore(bestParameterisedSplineDistance);

                        if (nodeIndex == nodes.size() - 1)
                        {
                            nodeEdges.push_back(nodes.at(static_cast<size_t>(nodeIndex)));
                        }
                        else
                        {
                            nodeEdges.push_back(nodes.at(static_cast<size_t>(nodeIndex)));
                            nodeEdges.push_back(nodes.at(static_cast<size_t>(nodeIndex + 1)));
                        }
                    }

                    return OfflineRoutingPointOnGraph(true,
                                                      bestPointOnSpline,
                                                      nodeEdges,
                                                      floorId,
                                                      bestFeatureId,
                                                      interiorId,
                                                      isPositionedOnNode);

                }
            }
        }
    }
}