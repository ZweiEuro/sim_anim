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
#include <Agui/FlowLayout.hpp>

#include <allegro5/allegro_native_dialog.h>
#include <stdlib.h>
#include <vector>
#include <ctime>
#include <iostream>
namespace mg8
{

  class SimpleActionListener : public agui::ActionListener
  {
  public:
    virtual void actionPerformed(const agui::ActionEvent &evt)
    {
      agui::Slider *slider = dynamic_cast<agui::Slider *>(evt.getSource());
      if (slider)
      {
        slider->setBackColor(agui::Color(slider->getValue(), slider->getValue(), slider->getValue()));
        return;
      }
      al_show_native_message_box(al_get_current_display(),
                                 "Agui Action Listener",
                                 "",
                                 "An Action Event has occured!",
                                 NULL, 0);
    }
  };

  class WidgetCreator
  {
  private:
    static WidgetCreator *_instance;

    SimpleActionListener simpleAL;
    agui::FlowLayout flow;
    agui::Button button;
    agui::CheckBox checkBox;
    agui::DropDown dropDown;
    agui::TextField textField;
    agui::Frame frame;
    agui::Gui *mGui;
    agui::RadioButton rButton[3];
    agui::RadioButtonGroup rGroup;
    agui::Slider slider;
    agui::ExtendedTextBox exTextBox;
    agui::TabbedPane tabbedPane;
    agui::Tab tab[3];
    agui::ListBox listBox;
    agui::ScrollPane scrollPane;
    agui::Button scrollButtons[15];

    ALLEGRO_EVENT_QUEUE *queue = NULL;
    bool done = false;
    agui::Gui *gui = NULL;
    agui::Allegro5Input *inputHandler = NULL;
    agui::Allegro5Graphics *graphicsHandler = NULL;

    agui::Font *defaultFont = NULL;

    WidgetCreator();
    ~WidgetCreator();

  public:
    static WidgetCreator *
    instance();
  };
}
