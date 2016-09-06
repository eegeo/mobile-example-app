// Copyright eeGeo Ltd (2012-2015), All Rights Reserved

#include "InitialExperienceIntroStep.h"
#include "IPersistentSettingsModel.h"
#include "ShowInitialExperienceIntroMessage.h"
#include "IWatermarkViewModel.h"
#include "SetWatermarkVisibilityMessage.h"

namespace
{
    const std::string InitialExperienceModel_HasStartedIntro = "InitialExperienceModel_HasStartedIntroKey";
    
    const std::string InitialExperienceModel_HasCompletedIntro = "InitialExperienceModel_HasCompletedIntroKey";
    
    bool GetKeyValue(ExampleApp::PersistentSettings::IPersistentSettingsModel& persistentSettings, const std::string& key)
    {
        bool result = false;
        
        if(!persistentSettings.TryGetValue(key, result))
        {
            result = false;
        }
        
        return result;
    }
}

namespace ExampleApp
{
    namespace InitialExperience
    {
        namespace SdkModel
        {
            InitialExperienceIntroStep::InitialExperienceIntroStep(ExampleAppMessaging::TMessageBus& messageBus,
                                                                   PersistentSettings::IPersistentSettingsModel& persistentSettings)
            : m_messageBus(messageBus)
            , m_persistentSettings(persistentSettings)
            , m_dismissedMessageHandler(this, &InitialExperienceIntroStep::OnIntroDismissed)
            , m_currentlyShowingIntro(false)
            {
                m_messageBus.SubscribeNative(m_dismissedMessageHandler);
            }
            
            InitialExperienceIntroStep::~InitialExperienceIntroStep()
            {
                m_messageBus.UnsubscribeNative(m_dismissedMessageHandler);
            }
            
            bool InitialExperienceIntroStep::HasStarted() const
            {
                return GetKeyValue(m_persistentSettings, InitialExperienceModel_HasStartedIntro);
            }
            
            bool InitialExperienceIntroStep::HasCompleted() const
            {
                bool result = GetKeyValue(m_persistentSettings, InitialExperienceModel_HasCompletedIntro);

                if(result)
                {
                	m_messageBus.Publish(WorldPins::WorldPinsVisibilityMessage(true));
                }

                return result;
            }
            
            void InitialExperienceIntroStep::PerformInitialExperience()
            {
                Eegeo_ASSERT(!HasCompleted(), "Cannot perform InitialExperienceDialogStep, has already completed.\n");
                m_persistentSettings.SetValue(InitialExperienceModel_HasStartedIntro, true);
            }
            
            void InitialExperienceIntroStep::ShowIntro()
            {
                m_messageBus.Publish(Watermark::SetWatermarkVisibilityMessage(false));
                m_currentlyShowingIntro = true;
                m_messageBus.Publish(ShowInitialExperienceIntroMessage());
            }
            
            void InitialExperienceIntroStep::Complete()
            {
                Eegeo_ASSERT(!HasCompleted(), "Cannot perform InitialExperienceDialogStep, has already completed.\n");
                m_persistentSettings.SetValue(InitialExperienceModel_HasCompletedIntro, true);
                m_messageBus.Publish(Watermark::SetWatermarkVisibilityMessage(true));
            }
            
            void InitialExperienceIntroStep::OnIntroDismissed(const InitialExperienceIntroDismissedMessage& message)
            {
                if(m_currentlyShowingIntro)
                {
                    Complete();
                    m_currentlyShowingIntro = false;
                }
            }
            
            void InitialExperienceIntroStep::Update(float deltaSeconds)
            {
                if(HasStarted() && !HasCompleted())
                {
                    if(!m_currentlyShowingIntro)
                    {
                        ShowIntro();
                    }
                }
            }

        }
    }
}
