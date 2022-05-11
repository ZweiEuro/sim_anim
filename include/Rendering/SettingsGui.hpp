#pragma once

#include <Agui/Agui.hpp>
#include <Agui/Backends/Allegro5/Allegro5.hpp>
#include <Agui/Backends/Allegro5/Allegro5Input.hpp>

#include <Agui/Widgets/Button/Button.hpp>
#include <Agui/Widgets/CheckBox/CheckBox.hpp>
#include <Agui/Widgets/DropDown/DropDown.hpp>
#include <Agui/Widgets/TextField/TextField.hpp>
#include <Agui/Widgets/Frame/Frame.hpp>
#include <Agui/Widgets/RadioButton/RadioButton.hpp>
#include <Agui/Widgets/RadioButton/RadioButtonGroup.hpp>
#include <Agui/Widgets/Slider/Slider.hpp>
#include <Agui/Widgets/TextBox/ExtendedTextBox.hpp>
#include <Agui/Widgets/Tab/TabbedPane.hpp>
#include <Agui/Widgets/ListBox/ListBox.hpp>
#include <Agui/Widgets/ScrollPane/ScrollPane.hpp>
#include <Agui/Widgets/Label/Label.hpp>

#include <Agui/FlowLayout.hpp>

#include <allegro5/allegro_native_dialog.h>
#include <stdlib.h>
#include <vector>
#include <ctime>
#include <iostream>
#include <thread>
#include <spdlog/spdlog.h>

#include "GameCore/GameManager.hpp"
namespace mg8
{

  class SimpleActionListener;
  class SettingsGUI : public agui::Gui
  {
  private:
    static SettingsGUI *m_instance;

    SimpleActionListener *m_action_listeneer = nullptr;
    /*agui::FlowLayout flow;
    agui::Button button;
    agui::CheckBox checkBox;
    agui::DropDown dropDown;
    agui::TextField textField;
    agui::RadioButton rButton[3];
    agui::RadioButtonGroup rGroup;
    agui::ExtendedTextBox exTextBox;
    agui::TabbedPane tabbedPane;
    agui::Tab tab[3];
    agui::ListBox listBox;
    agui::ScrollPane scrollPane;
    agui::Button scrollButtons[15];*/

    agui::Frame m_main_frame;

    agui::Label m_slider_title;
    agui::Slider m_slider_time_multiplier;

    SettingsGUI();

  public:
    static SettingsGUI *instance();

    std::atomic<float> m_slider_value = 1;
    agui::Label m_slider_value_display;
    agui::CheckBox m_debug_enabled;
  };

  class SimpleActionListener : public agui::ActionListener
  {
  public:
    virtual void actionPerformed(const agui::ActionEvent &evt)
    {
      agui::Slider *slider = dynamic_cast<agui::Slider *>(evt.getSource());
      if (slider)
      {
        auto &val = SettingsGUI::instance()->m_slider_value;
        val = ((float)slider->getValue()) / 100;
        SettingsGUI::instance()->m_slider_value_display.setText(std::to_string(val.load()).c_str());
        // SettingsGUI::instance()->m_slider_value_display.setText("0.05");

        /*spdlog::info("set to {} from {}",
                     SettingsGUI::instance()->m_slider_value_display.getText(),
                     std::to_string(val.load()));*/

        return;
      }
      agui::CheckBox *checkbox = dynamic_cast<agui::CheckBox *>(evt.getSource());
      if (checkbox)
      {
        bool is_checked = checkbox->checked();
        GameManager::instance()->debug_enabled = is_checked;
      }

      spdlog::info("button pushed");

      /*   al_show_native_message_box(al_get_current_display(),
                                    "Agui Action Listener",
                                    "",
                                    "An Action Event has occured!",
                                    NULL, 0);*/
    }
  };

}
