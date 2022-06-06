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
#include "GameCore/PhysicsManager.hpp"
#include "Rendering/Renderer.hpp"

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

    agui::Label m_voronoi_title;

    SettingsGUI();

  public:
    static SettingsGUI *instance();

    agui::Button m_voronoi_recalc;
    // general
    agui::CheckBox m_checkbox_debug_enabled;

    std::atomic<float> m_white_ball_power_value = config_default_white_ball_power;
    agui::Label m_title_white_ball_powerslider;
    agui::Slider m_slider_white_ball_power;
    agui::Label m_label_white_ball_power;

    // update rates
    std::atomic<float> m_time_delta_permultiplier_value = 1;
    agui::Label m_title_time_delta_slider;
    agui::Slider m_slider_time_multiplier;
    agui::Label m_label_time_multiplier;

    std::atomic<float> m_fps_value = config_default_fps;
    agui::Label m_title_fps_slider;
    agui::Slider m_slider_FPS;
    agui::Label m_label_FPS;

    std::atomic<float> m_pps_value = config_default_pps; // physics per second
    agui::Label m_title_pps_slider;
    agui::Slider m_slider_PPS;
    agui::Label m_label_PPS;

    std::atomic<float> m_h_value = config_default_h; // physics per second
    agui::Label m_title_h_slider;
    agui::Slider m_slider_h;
    agui::Label m_label_h;

    agui::CheckBox m_checkbox_table_friction;

    // gravity shit
    agui::CheckBox m_checkbox_forcefield;
    agui::CheckBox m_checkbox_object_path;

    std::atomic<bool> m_choice_euler = config_default_gravity;
    agui::RadioButton m_radio_kutta_euler[2];
    agui::RadioButtonGroup m_group_radio_kutta_euler;
  };

  class SimpleActionListener : public agui::ActionListener
  {
  public:
    void handleSlider(agui::Slider *slider)
    {
      if (slider == &SettingsGUI::instance()->m_slider_time_multiplier)
      {
        auto &val = SettingsGUI::instance()->m_time_delta_permultiplier_value;
        val = ((float)slider->getValue()) / 100;
        SettingsGUI::instance()->m_label_time_multiplier.setText(std::to_string(val.load()).c_str());
      }
      else if (slider == &SettingsGUI::instance()->m_slider_FPS)
      {
        printf("Set FPS: %d\n", slider->getValue());
        SettingsGUI::instance()->m_label_FPS.setText(std::to_string(slider->getValue()).c_str());

        Renderer::instance()->setFPS(slider->getValue());
      }
      else if (slider == &SettingsGUI::instance()->m_slider_PPS)
      {
        printf("Set PPS: %d\n", slider->getValue());
        SettingsGUI::instance()->m_label_PPS.setText(std::to_string(slider->getValue()).c_str());

        PhysicsManager::instance()->setPPS(slider->getValue());
      }
      else if (slider == &SettingsGUI::instance()->m_slider_h)
      {
        printf("Set h: %d\n", slider->getValue());
        auto &val = SettingsGUI::instance()->m_h_value;
        val = ((float)slider->getValue()) / 1000;

        SettingsGUI::instance()->m_label_h.setText(std::to_string(val.load()).c_str());
      }
      else if (slider == &SettingsGUI::instance()->m_slider_white_ball_power)
      {
        SettingsGUI::instance()->m_white_ball_power_value = slider->getValue();
        SettingsGUI::instance()->m_label_white_ball_power.setText(std::to_string(slider->getValue()).c_str());
      }
    }

    void handleCheckbox(agui::CheckBox *checkbox)
    {
      if (checkbox == &SettingsGUI::instance()->m_checkbox_debug_enabled)
      {
        printf("Set debug to %s\n", checkbox->checked() ? "True" : "False");
        GameManager::instance()->debug_enabled = checkbox->checked();
      }
      else if (checkbox == &SettingsGUI::instance()->m_checkbox_forcefield)
      {
        printf("Set forcefield to %s\n", checkbox->checked() ? "True" : "False");

        // GameManager::instance()->forcefield_enabled = checkbox->checked();
      }
      else if (checkbox == &SettingsGUI::instance()->m_checkbox_object_path)
      {
        printf("Set obj path to %s\n", checkbox->checked() ? "True" : "False");

        //  GameManager::instance()->object_path_enabled = checkbox->checked();
      }
    }

    virtual void actionPerformed(const agui::ActionEvent &evt)
    {
      agui::Slider *slider = dynamic_cast<agui::Slider *>(evt.getSource());
      if (slider)
      {
        handleSlider(slider);
        return;
      }
      agui::CheckBox *checkbox = dynamic_cast<agui::CheckBox *>(evt.getSource());
      if (checkbox)
      {
        handleCheckbox(checkbox);
        return;
      }

      agui::Button *btn = dynamic_cast<agui::Button *>(evt.getSource());
      if (btn)
      {
        if (btn->getText() == "enable")
        {
          btn->setText("disable");
          btn->resizeToContents();
        }
        else
        {
          btn->setText("enable");
          btn->resizeToContents();
        }

        GameManager::instance()->voronoi_recalc = true;
      }

      /*   al_show_native_message_box(al_get_current_display(),
                                    "Agui Action Listener",
                                    "",
                                    "An Action Event has occured!",
                                    NULL, 0);*/
      agui::RadioButton *button = dynamic_cast<agui::RadioButton *>(evt.getSource());
      if (button)
      {
        if (button == &SettingsGUI::instance()->m_radio_kutta_euler[0])
        {
          SettingsGUI::instance()->m_choice_euler = true;
        }
        else if (button == &SettingsGUI::instance()->m_radio_kutta_euler[1])
        {
          SettingsGUI::instance()->m_choice_euler = false;
        }
      }
    }
  };

}
