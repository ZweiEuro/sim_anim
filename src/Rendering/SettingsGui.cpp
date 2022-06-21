#include "Rendering/SettingsGui.hpp"

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
    m_main_frame.setSize(500, 120);
    m_main_frame.setLocation(config_start_resolution_w / 10, 10);
    m_main_frame.setText("Settings frame - Drag low right corner to resize");

    // Time delta slider

    float height = 0;
    float width[4] = {0, 50, 200, 250};

    { // time delta multiplier
      m_main_frame.add(&m_title_time_delta_slider);
      m_title_time_delta_slider.setText("TDM:");
      m_title_time_delta_slider.resizeToContents();
      m_title_time_delta_slider.setLocation(width[0], height);

      m_main_frame.add(&m_label_time_multiplier);
      m_label_time_multiplier.setText("0.123456789");
      m_label_time_multiplier.setText(std::to_string(m_time_delta_permultiplier_value.load()).c_str());
      m_label_time_multiplier.resizeToContents();
      m_label_time_multiplier.setLocation(width[1] + 10, height + 15);

      m_main_frame.add(&m_slider_time_multiplier);
      m_slider_time_multiplier.setSize(100, 50);
      m_slider_time_multiplier.setLocation(width[1], height);
      m_slider_time_multiplier.setMinValue(0);
      m_slider_time_multiplier.setMaxValue(100);
      m_slider_time_multiplier.setValue(100);
      m_slider_time_multiplier.setMarkerSize(agui::Dimension(10, 30));
      m_slider_time_multiplier.addActionListener(m_action_listeneer);
      m_slider_time_multiplier.resizeToContents();
      m_slider_time_multiplier.setBackColor(agui::Color(0, 0, 0, 0));

      height += 20;
    }

    m_main_frame.add(&m_checkbox_debug_enabled);
    m_checkbox_debug_enabled.setAutosizing(true);
    m_checkbox_debug_enabled.setText("Enable Debug View");
    m_checkbox_debug_enabled.setChecked(false);
    m_checkbox_debug_enabled.addActionListener(m_action_listeneer);
    m_checkbox_debug_enabled.setLocation(width[0], height += 20);

    m_main_frame.add(&m_checkbox_forcefield);
    m_checkbox_forcefield.setAutosizing(true);
    m_checkbox_forcefield.setText("Enable Forcefield");
    m_checkbox_forcefield.setChecked(false);
    m_checkbox_forcefield.addActionListener(m_action_listeneer);
    m_checkbox_forcefield.setLocation(width[0], height += 20);

    m_main_frame.add(&m_checkbox_object_path);
    m_checkbox_object_path.setAutosizing(true);
    m_checkbox_object_path.setText("Enable Object path");
    m_checkbox_object_path.setChecked(false);
    m_checkbox_object_path.addActionListener(m_action_listeneer);
    m_checkbox_object_path.setLocation(width[0], height += 20);

    m_main_frame.add(&m_checkbox_table_friction);
    m_checkbox_table_friction.setAutosizing(true);
    m_checkbox_table_friction.setText("Table Friction");
    m_checkbox_table_friction.setChecked(true);
    m_checkbox_table_friction.addActionListener(m_action_listeneer);
    m_checkbox_table_friction.setLocation(width[0], height += 20);

    m_main_frame.add(&m_checkbox_voronoi_recalc);
    m_checkbox_voronoi_recalc.setAutosizing(true);
    m_checkbox_voronoi_recalc.setText("Enable Voronoi Noise-Overlay");
    m_checkbox_voronoi_recalc.setChecked(false);
    m_checkbox_voronoi_recalc.addActionListener(m_action_listeneer);
    m_checkbox_voronoi_recalc.setLocation(width[0], height += 20);

    m_group_radio_kutta_euler.add(&m_radio_kutta_euler[0]);
    m_radio_kutta_euler[0].setText("Euler");
    m_radio_kutta_euler[0].setLocation(width[0], height += 20);
    m_radio_kutta_euler[0].setChecked(true);
    m_radio_kutta_euler[0].addActionListener(m_action_listeneer);
    m_radio_kutta_euler[0].setAutosizing(true);
    m_main_frame.add(&m_radio_kutta_euler[0]);

    m_main_frame.add(&m_radio_kutta_euler[1]);
    m_radio_kutta_euler[1].setText("Kutta");
    m_radio_kutta_euler[1].setLocation(width[0], height += 20);
    m_radio_kutta_euler[1].setChecked(true);
    m_radio_kutta_euler[1].addActionListener(m_action_listeneer);
    m_radio_kutta_euler[1].setAutosizing(true);
    m_group_radio_kutta_euler.add(&m_radio_kutta_euler[1]);

    height += 20;

    { // FPS
      m_main_frame.add(&m_title_fps_slider);
      m_title_fps_slider.setText("FPS: ");
      m_title_fps_slider.resizeToContents();
      m_title_fps_slider.setLocation(width[0], height);

      m_main_frame.add(&m_label_FPS);
      m_label_FPS.setText("0.123456789");
      m_label_FPS.setText(std::to_string(m_fps_value.load()).c_str());
      m_label_FPS.resizeToContents();
      m_label_FPS.setLocation(width[1] + 10, height + 10);

      m_main_frame.add(&m_slider_FPS);
      m_slider_FPS.setSize(100, 40);
      m_slider_FPS.setMinValue(1);
      m_slider_FPS.setMaxValue(100);
      m_slider_FPS.setValue(m_fps_value.load());
      m_slider_FPS.setMarkerSize(agui::Dimension(10, 30));
      m_slider_FPS.addActionListener(m_action_listeneer);
      m_slider_FPS.setLocation(width[1], height);
      m_slider_FPS.resizeToContents();
      m_slider_FPS.setBackColor(agui::Color(0, 0, 0, 0));

      height += 25;
    }
    { // PPS
      m_main_frame.add(&m_title_pps_slider);
      m_title_pps_slider.setText("PPS: ");
      m_title_pps_slider.resizeToContents();
      m_title_pps_slider.setLocation(width[0], height);

      m_main_frame.add(&m_label_PPS);
      m_label_PPS.setText("0.123456789");
      m_label_PPS.setText(std::to_string(m_pps_value.load()).c_str());
      m_label_PPS.resizeToContents();
      m_label_PPS.setLocation(width[1] + 10, height + 10);

      m_main_frame.add(&m_slider_PPS);
      m_slider_PPS.setSize(100, 40);
      m_slider_PPS.setMinValue(5);
      m_slider_PPS.setMaxValue(100);
      m_slider_PPS.setValue(m_pps_value.load());
      m_slider_PPS.setMarkerSize(agui::Dimension(10, 30));
      m_slider_PPS.addActionListener(m_action_listeneer);
      m_slider_PPS.setLocation(width[1], height);
      m_slider_PPS.resizeToContents();
      m_slider_PPS.setBackColor(agui::Color(0, 0, 0, 0));

      height += 25;
    }

    { // h
      m_main_frame.add(&m_title_h_slider);
      m_title_h_slider.setText("h: ");
      m_title_h_slider.resizeToContents();
      m_title_h_slider.setLocation(width[0], height);

      m_main_frame.add(&m_label_h);
      m_label_h.setText("0.123456789");
      m_label_h.setText(std::to_string(m_h_value.load()).c_str());
      m_label_h.resizeToContents();
      m_label_h.setLocation(width[1] + 10, height + 10);

      m_main_frame.add(&m_slider_h);
      m_slider_h.setSize(100, 40);
      m_slider_h.setMinValue(0);
      m_slider_h.setMaxValue(100);
      m_slider_h.setValue(m_h_value.load());
      m_slider_h.setMarkerSize(agui::Dimension(10, 30));
      m_slider_h.addActionListener(m_action_listeneer);
      m_slider_h.setLocation(width[1], height);
      m_slider_h.resizeToContents();
      m_slider_h.setBackColor(agui::Color(0, 0, 0, 0));

      height += 25;
    }

    { // ball power
      m_main_frame.add(&m_title_white_ball_powerslider);
      m_title_white_ball_powerslider.setText("BP: ");
      m_title_white_ball_powerslider.resizeToContents();
      m_title_white_ball_powerslider.setLocation(width[0], height);

      m_main_frame.add(&m_label_white_ball_power);
      m_label_white_ball_power.setText("0.123456789");
      m_label_white_ball_power.setText(std::to_string(m_white_ball_power_value.load()).c_str());
      m_label_white_ball_power.resizeToContents();
      m_label_white_ball_power.setLocation(width[1] + 10, height + 10);

      m_main_frame.add(&m_slider_white_ball_power);
      m_slider_white_ball_power.setSize(100, 40);
      m_slider_white_ball_power.setMinValue(0);
      m_slider_white_ball_power.setMaxValue(20);
      m_slider_white_ball_power.setValue(m_white_ball_power_value.load());
      m_slider_white_ball_power.setMarkerSize(agui::Dimension(10, 30));
      m_slider_white_ball_power.addActionListener(m_action_listeneer);
      m_slider_white_ball_power.setLocation(width[1], height);
      m_slider_white_ball_power.resizeToContents();
      m_slider_white_ball_power.setBackColor(agui::Color(0, 0, 0, 0));

      height += 25;
    }

    { // num voronoi Cells
      m_main_frame.add(&m_title_num_voronoi_cells_slider);
      m_title_num_voronoi_cells_slider.setText("Num Voronoi Cells: ");
      m_title_num_voronoi_cells_slider.resizeToContents();
      m_title_num_voronoi_cells_slider.setLocation(width[0], height + 10);

      m_main_frame.add(&m_label_num_voronoi_cells);
      m_label_num_voronoi_cells.setText("0.123456789");
      m_label_num_voronoi_cells.setText(std::to_string(m_num_voronoi_cells_value.load()).c_str());
      m_label_num_voronoi_cells.resizeToContents();
      m_label_num_voronoi_cells.setLocation(m_title_num_voronoi_cells_slider.getWidth() + 10, height + 10);

      m_main_frame.add(&m_slider_num_voronoi_cells);
      m_slider_num_voronoi_cells.setSize(100, 40);
      m_slider_num_voronoi_cells.setMinValue(5);
      m_slider_num_voronoi_cells.setMaxValue(30);
      m_slider_num_voronoi_cells.setValue(m_num_voronoi_cells_value.load());
      m_slider_num_voronoi_cells.setMarkerSize(agui::Dimension(10, 30));
      m_slider_num_voronoi_cells.addActionListener(m_action_listeneer);
      m_slider_num_voronoi_cells.setLocation(m_title_num_voronoi_cells_slider.getWidth(), height);
      m_slider_num_voronoi_cells.resizeToContents();
      m_slider_num_voronoi_cells.setBackColor(agui::Color(0, 0, 0, 0));

      height += 25;
    }
    { // Gravity Well despawn Time
      m_main_frame.add(&m_title_gravity_well_despawn_time_slider);
      m_title_gravity_well_despawn_time_slider.setText("Gravity Well Despawn Time (sec): ");
      m_title_gravity_well_despawn_time_slider.resizeToContents();
      m_title_gravity_well_despawn_time_slider.setLocation(width[0], height + 10);

      m_main_frame.add(&m_label_gravity_well_despawn_time);
      m_label_gravity_well_despawn_time.setText("0.123456789");
      m_label_gravity_well_despawn_time.setText(std::to_string(m_gravity_well_despawn_time_value.load()).c_str());
      m_label_gravity_well_despawn_time.resizeToContents();
      m_label_gravity_well_despawn_time.setLocation(m_title_gravity_well_despawn_time_slider.getWidth() + 10, height + 10);

      m_main_frame.add(&m_slider_gravity_well_despawn_time);
      m_slider_gravity_well_despawn_time.setSize(100, 40);
      m_slider_gravity_well_despawn_time.setMinValue(1);
      m_slider_gravity_well_despawn_time.setMaxValue(30);
      m_slider_gravity_well_despawn_time.setValue(m_gravity_well_despawn_time_value.load());
      m_slider_gravity_well_despawn_time.setMarkerSize(agui::Dimension(10, 30));
      m_slider_gravity_well_despawn_time.addActionListener(m_action_listeneer);
      m_slider_gravity_well_despawn_time.setLocation(m_title_gravity_well_despawn_time_slider.getWidth(), height);
      m_slider_gravity_well_despawn_time.resizeToContents();
      m_slider_gravity_well_despawn_time.setBackColor(agui::Color(0, 0, 0, 0));

      height += 25;
    }
    /*{
      m_main_frame.add(&m_voronoi_title);
      m_voronoi_title.setText("Voronoi noise");
      m_voronoi_title.resizeToContents();
      auto voronoi_title_dims = m_voronoi_title.getSize();
      m_voronoi_title.setLocation(width[0], height + 10);

      m_main_frame.add(&m_button_voronoi_recalc);
      m_button_voronoi_recalc.setLocation(width[0] + voronoi_title_dims.getWidth(), height + 10);

      m_button_voronoi_recalc.setText("enable");
      m_button_voronoi_recalc.resizeToContents();
      m_button_voronoi_recalc.setBackColor({0, 255, 255});
      m_button_voronoi_recalc.addActionListener(m_action_listeneer);
      height += 25;
    }*/

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