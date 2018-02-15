// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#include "InitialExperienceIntroViewModule.h"
#include "InitialExperienceIntroView.h"
#include "InitialExperienceIntroController.h"

namespace ExampleApp
{
    namespace InitialExperience
    {
        namespace View
        {
            InitialExperienceIntroViewModule::InitialExperienceIntroViewModule(WindowsNativeState& nativeState,
                                                                               ExampleAppMessaging::TMessageBus& messageBus,
                                                                               bool isInKioskMode,
                                                                               CameraTransitions::SdkModel::ICameraTransitionController& cameraTransitionController,
																			   const Compass::SdkModel::ICompassModule& compassModule)
            {
                m_pView = Eegeo_NEW(InitialExperienceIntroView)(nativeState, messageBus, isInKioskMode);
                m_pController = Eegeo_NEW(InitialExperienceIntroController)(*m_pView, messageBus, isInKioskMode, cameraTransitionController, compassModule);
            }

            InitialExperienceIntroViewModule::~InitialExperienceIntroViewModule()
            {
                Eegeo_DELETE m_pController;
                Eegeo_DELETE m_pView;
            }
        }
    }
}
