#include "Rendering/SettingsGui.hpp"
#include "configuration.hpp"
#include "Rendering/Renderer.hpp"

#include <string>
#include <spdlog/spdlog.h>
namespace mg8
{
  SettingsGUI *SettingsGUI::m_instance = nullptr;

  SettingsGUI *SettingsGUI::instance()
  {
    if (!m_instance)
    {
      m_instance = new SettingsGUI();
    }
    return m_instance;
  }

  SettingsGUI::SettingsGUI()
  {
    m_action_listeneer = new SimpleActionListener();
    setInput(Renderer::instance()->get_agui_input_handler());
    setGraphics(Renderer::instance()->get_agui_graphics_handler());

    // main frame dimensions
    add(&m_main_frame);
    m_main_frame.setSize(220, 120);
    m_main_frame.setLocation(config_start_resolution_w / 10, 10);
    m_main_frame.setText("Settings frame");

    // Time delta slider

    m_main_frame.add(&m_slider_title);
    m_slider_title.setText("Time delta multiplier (Update Rate):");
    m_slider_title.resizeToContents();
    auto slider_title_dims = m_slider_title.getSize();
    m_slider_title.setLocation(0, 10);
    auto complete_width = slider_title_dims.getWidth();

    m_main_frame.add(&m_slider_time_multiplier);
    m_slider_time_multiplier.setSize(100, 50);
    m_slider_time_multiplier.setLocation(slider_title_dims.getWidth(), 0);
    m_slider_time_multiplier.setMinValue(0);
    m_slider_time_multiplier.setMaxValue(100);
    m_slider_time_multiplier.setValue(100);
    m_slider_time_multiplier.setMarkerSize(agui::Dimension(10, 30));
    m_slider_time_multiplier.addActionListener(m_action_listeneer);
    m_slider_time_multiplier.resizeToContents();
    auto slider_dims = m_slider_time_multiplier.getSize();
    complete_width += slider_dims.getWidth();

    m_main_frame.add(&m_slider_value_display);
    m_slider_value_display.setText("0.123456789");
    m_slider_value_display.setText(std::to_string(m_slider_value.load()).c_str());

    m_slider_value_display.resizeToContents();
    auto slider_value_dims = m_slider_value_display.getSize();
    m_slider_value_display.setLocation(complete_width + 10, 10);
    complete_width += slider_value_dims.getWidth();

    m_main_frame.setSize(complete_width + 50, 120);

    m_main_frame.add(&m_debug_enabled);
    m_debug_enabled.setLocation(10, 50);
    m_debug_enabled.setAutosizing(true);
    m_debug_enabled.setText("Enable Debug View");
    m_debug_enabled.setCheckBoxAlignment(agui::ALIGN_MIDDLE_LEFT);
    m_debug_enabled.setChecked(GameManager::instance()->debug_enabled);
    m_debug_enabled.addActionListener(m_action_listeneer);

    /*std::stringstream ss;
    for (int i = 0; i < 3; ++i)
    {
      ss.str("");
      ss.clear();
      ss << "Sample Radio Button ";
      ss << i;

      rGroup.add(&rButton[i]);
      rButton[i].setAutosizing(true);
      rButton[i].setText(ss.str());
      frame.add(&rButton[i]);
      // rButton[i].setLocation(0, 30 * i);
      rButton[i].addActionListener(&simpleAL);
    }*/

    /* add(&flow);
     flow.add(&button);
     button.setSize(80, 40);
     button.setText("Push Me");
     button.addActionListener(&simpleAL);

      flow.add(&checkBox);
      checkBox.setAutosizing(true);
      checkBox.setText("Show me a message box");
      checkBox.setCheckBoxAlignment(agui::ALIGN_MIDDLE_LEFT);
      checkBox.addActionListener(&simpleAL);

      flow.add(&dropDown);
      dropDown.setText("Select Item");
      dropDown.setSize(120, 30);
      dropDown.addItem("Apples");
      dropDown.addItem("Oranges");
      dropDown.addItem("Grapes");
      dropDown.addItem("Peaches");
      dropDown.addActionListener(&simpleAL);

      flow.add(&textField);
      textField.setText("Press Enter!");
      textField.resizeToContents();
      textField.addActionListener(&simpleAL);



      flow.add(&slider);
      slider.setSize(100, 36);
      slider.setMaxValue(255);
      slider.setMarkerSize(agui::Dimension(10, 30));
      slider.addActionListener(&simpleAL);

      exTextBox.setWordWrap(true);
      exTextBox.setText("Welcome \n");
      exTextBox.setCurrentColor(agui::Color(255, 0, 0));
      exTextBox.appendText("WARNING!!!\n\n");
      exTextBox.setCurrentColor(agui::Color(255, 0, 255));
      exTextBox.appendText("Viewing text in magenta is not recommended\n\n");
      exTextBox.setCurrentColor(agui::Color(100, 0, 200));
      exTextBox.appendText("Copyright (c) 2011 Joshua Larouche \n \
      \
      License: (BSD) \n \
      Redistribution and use in source and binary forms, with or without \
      modification, are permitted provided that the following conditions \
      are met: \n \
      1. Redistributions of source code must retain the above copyright \
      notice, this list of conditions and the following disclaimer. \n \
      2. Redistributions in binary form must reproduce the above copyright \
      notice, this list of conditions and the following disclaimer in \
      the documentation and/or other materials provided with the distribution. \n \
      3. Neither the name of Agui nor the names of its contributors may \
      be used to endorse or promote products derived from this software \
      without specific prior written permission. \n\n \
      ");

      exTextBox.setCurrentColor(agui::Color(255, 128, 0));
      exTextBox.appendText("THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \
      \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT \
      LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR \
      A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT \
      OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, \
      SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED \
      TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR \
      PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF \
      LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING \
      NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS \
      SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. \
      ");
      exTextBox.setSelection(84, 100);
      exTextBox.setSelectionColor(agui::Color(255, 0, 0));
      exTextBox.setSelection(0, 0);
      exTextBox.setTextAlignment(agui::ALIGN_CENTER);
      exTextBox.setSize(600, 300);
      exTextBox.positionCaret(0, 0);

      tabbedPane.setSize(agui::Dimension(300, 400));
      flow.add(&tabbedPane);
      tab[0].setText("Text Box");
      tab[1].setText("List Box");
      tab[2].setText("Scroll Pane");
      tabbedPane.addTab(&tab[0], &exTextBox);
      tabbedPane.addTab(&tab[1], &listBox);
      tabbedPane.addTab(&tab[2], &scrollPane);

      tabbedPane.setResizeTabContent(true);

      for (int i = 0; i < 250; ++i)
      {
        ss.str("");
        ss.clear();
        ss << "Item ";
        ss << i;

        listBox.addItem(ss.str());
      }

      listBox.setMultiselectExtended(true);

      int count = 0;
      for (int i = 0; i < 3; ++i)
      {
        for (int j = 0; j < 5; ++j)
        {

          ss.str("");
          ss.clear();
          ss << "Scroll Button ";
          ss << i + 1;
          ss << " , ";
          ss << j + 1;

          scrollPane.add(&scrollButtons[count]);
          scrollButtons[count].setSize(200, 100);
          scrollButtons[count].setLocation(210 * i, 110 * j);
          scrollButtons[count].setText(ss.str());

          count++;
        }
      }*/
  }

}