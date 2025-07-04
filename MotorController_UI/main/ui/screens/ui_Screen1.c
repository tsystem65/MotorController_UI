#include "../ui.h"

static const lv_btnmatrix_ctrl_t custom_styles[] = {
    1, 1, 1, 1, // "1", "2", "3", "\n"
    1, 1, 1, 1, // "4", "5", "6", "\n"
    1, 1, 1, 1, // "7", "8", "9", "\n"
    1, 1, 1, 1                         // LV_SYMBOL_BACKSPACE, "0", LV_SYMBOL_OK, "Close"
};

static const char *custom_map[] = {
    "1", "2", "3", "\n",
    "4", "5", "6", "\n",
    "7", "8", "9", "\n",
    ".", "0", LV_SYMBOL_BACKSPACE, LV_SYMBOL_OK, ""
};

static void kb_event_handler(lv_event_t *e) {
    lv_obj_t *kb = lv_event_get_target(e);
    lv_obj_t *ta = lv_keyboard_get_textarea(kb);
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_READY) {
        LV_LOG_USER("Enter pressed. Text: %s", lv_textarea_get_text(ta));
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    } else if (code == LV_EVENT_VALUE_CHANGED) {
        const char *txt = lv_keyboard_get_btn_text(kb, lv_keyboard_get_selected_btn(kb));
        if (txt && strcmp(txt, "Close") == 0) {
            LV_LOG_USER("Close pressed");
            lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

static void ta_focus_handler(lv_event_t *e) {
    lv_obj_t *ta = lv_event_get_target(e);
    lv_obj_t *kb = lv_event_get_user_data(e);

    lv_keyboard_set_textarea(kb, ta); // Прив’язуємо клавіатуру до натиснутого поля
    lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN); // Показуємо клавіатуру
    lv_obj_add_state(ta, LV_STATE_FOCUSED); // Фокус на полі
}

void ui_Screen1_screen_init(void)
{
   //LV_FONT_DECLARE(lv_font_montserrat_18);
    static char btn_label_buf[64];

    static lv_style_t btn_style, btn_pr_style;
    lv_style_init(&btn_style);
    lv_style_init(&btn_pr_style);

    // 2. Стиль кнопки у звичайному стані
    lv_style_set_radius(&btn_style,         8);                      // округлі кути
    lv_style_set_bg_color(&btn_style,       lv_color_hex(0x008BF5));        // фон
    lv_style_set_bg_opa(&btn_style,         LV_OPA_10);
    lv_style_set_border_color(&btn_style,   lv_color_hex(0x008BF5));  // колір рамки
    lv_style_set_border_width(&btn_style,   1);
    lv_style_set_shadow_color(&btn_style,   lv_color_hex(0x008BF5));  // glow-ефект
    lv_style_set_shadow_width(&btn_style,   10);
    lv_style_set_shadow_ofs_x(&btn_style,   0);
    lv_style_set_shadow_ofs_y(&btn_style,   0);
    lv_style_set_shadow_spread(&btn_style,  2);
    lv_style_set_shadow_opa(&btn_style,     LV_OPA_40);

    // 3. Стиль кнопки у натиснутому стані
    lv_style_set_bg_color(&btn_pr_style,    lv_color_hex(0x008BF5)); // темніший фон
    lv_style_set_bg_opa(&btn_pr_style,      LV_OPA_30);
    lv_style_set_border_color(&btn_pr_style,lv_color_hex(0x008BF5));
    lv_style_set_border_width(&btn_pr_style,2);

    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);
    // lv_obj_set_layout(ui_Screen1, LV_LAYOUT_FLEX);
    // lv_obj_set_flex_flow(ui_Screen1, LV_FLEX_FLOW_COLUMN);
    // lv_obj_set_flex_align(ui_Screen1, LV_ALIGN_TOP_MID, LV_ALIGN_CENTER, LV_ALIGN_DEFAULT);
    // lv_obj_set_style_pad_all(ui_Screen1, 0, 0); 

    cont_main = lv_obj_create(ui_Screen1);
    lv_obj_clear_flag(cont_main, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(cont_main, LV_PCT(100), LV_PCT(100));
    //lv_obj_clear_flag(cont_main, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_layout(cont_main, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(cont_main, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont_main, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_BETWEEN);
    lv_obj_set_style_pad_all(cont_main, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_gap(cont_main, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(cont_main, lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_border_width(cont_main, 2, 0);
    lv_obj_set_style_border_color(cont_main, lv_color_white(), 0);
    lv_obj_set_style_border_opa(cont_main, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * cont_main_side_bar = lv_obj_create(cont_main);
    lv_obj_set_size(cont_main_side_bar, LV_PCT(7), LV_PCT(100));
    lv_obj_set_flex_flow(cont_main_side_bar, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont_main_side_bar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_BETWEEN);
    lv_obj_set_style_pad_all(cont_main_side_bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(cont_main_side_bar, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_gap(cont_main_side_bar, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cont_main_side_bar, 1, 0);
    lv_obj_set_style_border_color(cont_main_side_bar, lv_color_hex(0x303030), 0);
    lv_obj_set_style_border_side(cont_main_side_bar, LV_BORDER_SIDE_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cont_main_side_bar, LV_OPA_TRANSP, 0);

    lv_obj_t *wifi_btn = lv_btn_create(cont_main_side_bar);
    lv_obj_set_size(wifi_btn, 40, 40);
    lv_obj_add_style(wifi_btn, &btn_style, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(wifi_btn, &btn_pr_style, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_event_cb(wifi_btn, reset_button_clicked, LV_EVENT_CLICKED, NULL);

    lv_obj_t *wifi_btn_label = lv_label_create(wifi_btn);
    lv_label_set_text(wifi_btn_label, LV_SYMBOL_WIFI);
    lv_obj_set_style_text_font(wifi_btn_label,
                           &lv_font_montserrat_18,
                           LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_center(wifi_btn_label);

    lv_obj_t *notify_btn = lv_btn_create(cont_main_side_bar);
    lv_obj_set_size(notify_btn, 40, 40);
    lv_obj_add_style(notify_btn, &btn_style, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(notify_btn, &btn_pr_style, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_event_cb(notify_btn, reset_button_clicked, LV_EVENT_CLICKED, NULL);

    lv_obj_t *notify_btn_label = lv_label_create(notify_btn);
    lv_label_set_text(notify_btn_label, LV_SYMBOL_BELL);
    lv_obj_set_style_text_font(notify_btn_label,
                           &lv_font_montserrat_18,
                           LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_center(notify_btn_label);
    

    lv_obj_t * cont_main_left = lv_obj_create(cont_main);
    lv_obj_set_size(cont_main_left, LV_PCT(58), LV_PCT(100));
    lv_obj_set_flex_flow(cont_main_left, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont_main_left, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_BETWEEN);
    lv_obj_set_style_pad_all(cont_main_left, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cont_main_left, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_gap(cont_main_left, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(cont_main_left, 1, 0);
    lv_obj_set_style_border_color(cont_main_left, lv_color_hex(0x303030), 0);
    lv_obj_set_style_border_side(cont_main_left, LV_BORDER_SIDE_RIGHT, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(cont_main_left, LV_OPA_TRANSP, 0);

    lv_obj_t * cont_main_right = lv_obj_create(cont_main);
    lv_obj_set_size(cont_main_right, LV_PCT(35), LV_PCT(100));
    lv_obj_set_flex_flow(cont_main_right, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont_main_right, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_BETWEEN);
    lv_obj_set_style_pad_all(cont_main_right, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(cont_main_left, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_gap(cont_main_right, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_border_width(cont_main_right, 2, 0);
    //lv_obj_set_style_border_color(cont_main_right, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(cont_main_right, LV_OPA_TRANSP, 0);
    
    //lv_obj_set_style_border_opa(cont_main_left, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(cont_main_right, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);


    // lv_obj_t * operation_params_cont = lv_obj_create(cont_main_left);
    // lv_obj_set_size(operation_params_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    // lv_obj_set_align(operation_params_cont, LV_ALIGN_TOP_MID);
    // lv_obj_set_layout(operation_params_cont, LV_LAYOUT_FLEX);
    // lv_obj_set_flex_flow(operation_params_cont, LV_FLEX_FLOW_ROW);
    // lv_obj_set_flex_align(operation_params_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    // lv_obj_set_style_pad_row(operation_params_cont, 15, 0);
    // //lv_obj_set_style_border_opa(operation_params_cont, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    // //lv_obj_set_style_bg_opa(operation_params_cont, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    // ////lv_obj_set_style_border_opa(operation_params_cont, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    // //lv_obj_clear_flag(operation_params_cont, LV_OBJ_FLAG_SCROLLABLE);
    
    operation_params_input_cont = lv_obj_create(cont_main_left);
    lv_obj_set_size(operation_params_input_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(operation_params_input_cont, LV_OPA_TRANSP, 0);
    //lv_obj_clear_flag(operation_params_input_cont, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_border_opa(operation_params_input_cont, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Визначаємо колонки (лейбл: 180px, поле вводу: 220px) і рядки (по 50px)
    static lv_coord_t operation_params_input_cont_col_dsc[] = {210, 190, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t operation_params_input_cont_row_dsc[] = {50, 50, 50, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(operation_params_input_cont, operation_params_input_cont_col_dsc, operation_params_input_cont_row_dsc);

    // 2. Створення елементів безпосередньо в сітці
    // Перший рядок - Rotation Speed

    static lv_style_t st_def, st_focus;
    lv_style_init(&st_def);
    lv_style_set_bg_opa(&st_def,      LV_OPA_TRANSP);
    lv_style_set_border_width(&st_def, 1);
    lv_style_set_border_color(&st_def, lv_color_hex(0x008BF5));

    lv_style_init(&st_focus);
    lv_style_set_border_width(&st_focus, 1);
    lv_style_set_border_color(&st_focus, lv_color_hex(0xAEDCFF));;
    lv_style_set_shadow_color(&st_focus, lv_color_hex(0x9BC6FF));
    lv_style_set_shadow_width(&st_focus, 10);
    lv_style_set_shadow_ofs_x(&st_focus, 0);
    lv_style_set_shadow_ofs_y(&st_focus, 0);
    lv_style_set_shadow_spread(&st_focus, 2);
    lv_style_set_shadow_opa(&st_focus, LV_OPA_50);

    rotate_per_sec_label = lv_label_create(operation_params_input_cont);
    lv_label_set_text(rotate_per_sec_label, "Rotation Speed (rps):");
    lv_obj_set_style_text_font(rotate_per_sec_label,
                           &lv_font_montserrat_18,
                           LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_grid_cell(rotate_per_sec_label, 
        LV_GRID_ALIGN_START, 0, 1,  // Колонка 0, ширина 1
        LV_GRID_ALIGN_CENTER, 0, 1   // Рядок 0, висота 1
    );

    rotate_per_sec_entry = lv_textarea_create(operation_params_input_cont);
    lv_textarea_set_one_line(rotate_per_sec_entry, true);
    lv_obj_set_grid_cell(rotate_per_sec_entry,
        LV_GRID_ALIGN_STRETCH, 1, 1, // Розтягнути на всю колонку
        LV_GRID_ALIGN_CENTER, 0, 1
    );

    lv_obj_add_style(rotate_per_sec_entry, &st_def, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(rotate_per_sec_entry, &st_focus, LV_PART_MAIN | LV_STATE_FOCUSED);
    //lv_obj_add_style(rotate_per_sec_entry, &op_params_ta_style, LV_PART_PLACEHOLDER | LV_STATE_DEFAULT);

    // Другий рядок - Winding Step
    carriage_movement_label = lv_label_create(operation_params_input_cont);
    lv_label_set_text(carriage_movement_label, "Winding Step (mm):");
    lv_obj_set_style_text_font(carriage_movement_label,
                           &lv_font_montserrat_18,
                           LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_grid_cell(carriage_movement_label,
        LV_GRID_ALIGN_START, 0, 1,
        LV_GRID_ALIGN_CENTER, 1, 1
    );

    carriage_movement_entry = lv_textarea_create(operation_params_input_cont);
    lv_textarea_set_one_line(carriage_movement_entry, true);  // Виправлено: було rotate_per_sec_entry
    lv_obj_set_grid_cell(carriage_movement_entry,
        LV_GRID_ALIGN_STRETCH, 1, 1,
        LV_GRID_ALIGN_CENTER, 1, 1
    );

    lv_obj_add_style(carriage_movement_entry, &st_def, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(carriage_movement_entry, &st_focus, LV_PART_MAIN | LV_STATE_FOCUSED);

    // Третій рядок - Total Length
    general_winding_length_label = lv_label_create(operation_params_input_cont);
    lv_label_set_text(general_winding_length_label, "Total Length (m):");
    lv_obj_set_style_text_font(general_winding_length_label,
                           &lv_font_montserrat_18,
                           LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_grid_cell(general_winding_length_label,
        LV_GRID_ALIGN_START, 0, 1,
        LV_GRID_ALIGN_CENTER, 2, 1
    );

    general_winding_length_entry = lv_textarea_create(operation_params_input_cont);
    lv_textarea_set_one_line(general_winding_length_entry, true);
    lv_obj_set_grid_cell(general_winding_length_entry,
        LV_GRID_ALIGN_STRETCH, 1, 1,
        LV_GRID_ALIGN_CENTER, 2, 1
    );

    lv_obj_add_style(general_winding_length_entry, &st_def, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(general_winding_length_entry, &st_focus, LV_PART_MAIN | LV_STATE_FOCUSED);

    // lv_obj_t * operation_params_cont_separator = lv_line_create(operation_params_cont);
    // static lv_point_t operation_params_cont_separator_points[] = {{0, 0}, {0, 200}};  // Вертикальна лінія 100px
    // lv_line_set_points(operation_params_cont_separator, operation_params_cont_separator_points, 2);
    // lv_obj_set_style_line_width(operation_params_cont_separator, 1, 0);

    cont_operation_mode = lv_obj_create(cont_main_right);
    lv_obj_set_size(cont_operation_mode, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_layout(cont_operation_mode, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(cont_operation_mode, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont_operation_mode, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(cont_operation_mode, 15, 0);
    lv_obj_set_style_bg_opa(cont_operation_mode, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_bottom(cont_operation_mode, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(cont_operation_mode, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * operation_mode_header_label = lv_label_create(cont_operation_mode);
    lv_label_set_text(operation_mode_header_label, "-- Operation Modes --");
    lv_obj_set_style_text_font(operation_mode_header_label,
                           &lv_font_montserrat_18,
                           LV_PART_MAIN | LV_STATE_DEFAULT);

    grinding_mode_checkbox = lv_checkbox_create(cont_operation_mode);
    lv_checkbox_set_text(grinding_mode_checkbox, " Grinding mode");
    lv_obj_add_event_cb(grinding_mode_checkbox, grinding_mode_checkbox_handler, LV_EVENT_VALUE_CHANGED, NULL);

    conical_winding_checkbox = lv_checkbox_create(cont_operation_mode);
    lv_checkbox_set_text(conical_winding_checkbox, " Conical winding");
    lv_obj_add_event_cb(conical_winding_checkbox, conical_winding_checkbox_handler, LV_EVENT_VALUE_CHANGED, NULL);

    // revers_conical_winding_checkbox = lv_checkbox_create(cont_operation_mode);
    // lv_checkbox_set_text(revers_conical_winding_checkbox, " Revers conical");
    // lv_obj_add_event_cb(revers_conical_winding_checkbox, revers_conical_winding_checkbox_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *revers_conical_mode_cont = lv_obj_create(cont_operation_mode);
    lv_obj_set_size(revers_conical_mode_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(revers_conical_mode_cont, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_layout(revers_conical_mode_cont, LV_LAYOUT_GRID);
    lv_obj_set_style_pad_all(revers_conical_mode_cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(revers_conical_mode_cont, -3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_gap(revers_conical_mode_cont, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(revers_conical_mode_cont, LV_OBJ_FLAG_SCROLLABLE);
    // lv_obj_set_flex_flow(revers_conical_mode_cont, LV_FLEX_FLOW_ROW);
    // lv_obj_set_flex_align(revers_conical_mode_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    static lv_coord_t revers_conical_mode_cont_col_dsc[] = {90, 90, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t revers_conical_mode_cont_row_dsc[] = {30, 50, 50, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(revers_conical_mode_cont, revers_conical_mode_cont_col_dsc, revers_conical_mode_cont_row_dsc);
    
    // Видаляємо всі відступи
    //lv_obj_set_style_pad_all(revers_conical_mode_cont, 0, 0); // Зовнішні відступи (padding)
    //lv_obj_set_style_pad_gap(revers_conical_mode_cont, 0, 0); // Відступ між рядками
    
    // Видаляємо рамку
    lv_obj_set_style_border_width(revers_conical_mode_cont, 0, 0);
    
    revers_conical_winding_checkbox = lv_checkbox_create(revers_conical_mode_cont);
    lv_checkbox_set_text(revers_conical_winding_checkbox, " Revers conical");
    lv_obj_add_event_cb(revers_conical_winding_checkbox, revers_conical_winding_checkbox_handler, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_set_grid_cell(revers_conical_winding_checkbox,
        LV_GRID_ALIGN_STRETCH, 0, 2,
        LV_GRID_ALIGN_CENTER, 0, 1
    );

    revers_conical_delay_mm_entry = lv_textarea_create(revers_conical_mode_cont);
    lv_textarea_set_one_line(revers_conical_delay_mm_entry, true);
    lv_obj_add_style(revers_conical_delay_mm_entry, &st_def, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(revers_conical_delay_mm_entry, &st_focus, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_textarea_set_placeholder_text(revers_conical_delay_mm_entry, "Step (mm)");
    lv_obj_set_grid_cell(revers_conical_delay_mm_entry,
        LV_GRID_ALIGN_STRETCH, 0, 1,
        LV_GRID_ALIGN_CENTER, 1, 1
    );
    
    revers_conical_layers_entry = lv_textarea_create(revers_conical_mode_cont);
    lv_textarea_set_one_line(revers_conical_layers_entry, true);
    lv_obj_add_style(revers_conical_layers_entry, &st_def, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(revers_conical_layers_entry, &st_focus, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_textarea_set_placeholder_text(revers_conical_layers_entry, "Layers");
    lv_obj_set_grid_cell(revers_conical_layers_entry,
        LV_GRID_ALIGN_STRETCH, 1, 1,
        LV_GRID_ALIGN_CENTER, 1, 1
    );

    lv_obj_t * accept_revers_conical_params_btn = lv_btn_create(revers_conical_mode_cont);
    lv_obj_add_style(accept_revers_conical_params_btn, &btn_style,    LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(accept_revers_conical_params_btn, &btn_pr_style, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_grid_cell(accept_revers_conical_params_btn,
        LV_GRID_ALIGN_STRETCH, 0, 2,
        LV_GRID_ALIGN_CENTER, 2, 1
    );
    lv_obj_add_event_cb(accept_revers_conical_params_btn, accept_revers_conical_params_btn_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * accept_revers_conical_params_btn_label = lv_label_create(accept_revers_conical_params_btn);
    lv_obj_set_style_text_color(accept_revers_conical_params_btn_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(accept_revers_conical_params_btn_label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_snprintf(btn_label_buf, sizeof(btn_label_buf) - 1, " %s  ACCEPT", LV_SYMBOL_OK);
    lv_label_set_text(accept_revers_conical_params_btn_label, btn_label_buf);
    lv_obj_set_align(accept_revers_conical_params_btn_label, LV_ALIGN_CENTER);

    buttons_telemetry_cont = lv_obj_create(cont_main_left);
    lv_obj_set_size(buttons_telemetry_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_align(buttons_telemetry_cont, LV_ALIGN_TOP_MID);
    lv_obj_set_style_pad_row(buttons_telemetry_cont, 0, 0);
    lv_obj_set_style_pad_top(buttons_telemetry_cont, 0, 0);
    lv_obj_set_style_bg_opa(buttons_telemetry_cont, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(buttons_telemetry_cont, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(buttons_telemetry_cont, LV_OBJ_FLAG_SCROLLABLE);

    control_buttons_cont = lv_obj_create(buttons_telemetry_cont);
    lv_obj_set_size(control_buttons_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(control_buttons_cont, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(control_buttons_cont, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    static lv_coord_t control_buttons_cont_col_dsc[] = {130, 130, 130, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t control_buttons_cont_row_dsc[] = {90, 90, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(control_buttons_cont, control_buttons_cont_col_dsc, control_buttons_cont_row_dsc);

    start_button = lv_btn_create(control_buttons_cont);
    lv_obj_set_size(start_button, 200, 90);
    lv_obj_add_style(start_button, &btn_style,    LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(start_button, &btn_pr_style, LV_PART_MAIN | LV_STATE_PRESSED);

    //////// DEFAULT STATE STYLES ////////
    lv_obj_set_style_bg_color(start_button,       lv_color_hex(0x00D92F), LV_PART_MAIN | LV_STATE_DEFAULT);        // фон
    lv_obj_set_style_border_color(start_button,   lv_color_hex(0x00D92F), LV_PART_MAIN | LV_STATE_DEFAULT);  // колір рамки
    lv_obj_set_style_shadow_color(start_button,   lv_color_hex(0x00D92F), LV_PART_MAIN | LV_STATE_DEFAULT);  // glow-ефект

    //////// PRESSED STATE STYLES ////////
    lv_obj_set_style_bg_color(start_button,    lv_color_hex(0x00D92F), LV_PART_MAIN | LV_STATE_PRESSED); // темніший фон
    lv_obj_set_style_bg_opa(start_button,      LV_OPA_30, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_border_color(start_button, lv_color_hex(0x00D92F), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_border_width(start_button, 2, LV_PART_MAIN | LV_STATE_PRESSED);

    lv_obj_add_event_cb(start_button, start_button_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_set_grid_cell(start_button,
    LV_GRID_ALIGN_STRETCH, 0, 1,  // кол.0, span=1
    LV_GRID_ALIGN_CENTER, 0, 1    // ряд.0, span=1
    );

    start_button_label = lv_label_create(start_button);
    lv_snprintf(btn_label_buf, sizeof(btn_label_buf) - 1, " %s  START", LV_SYMBOL_PLAY);
    lv_label_set_text(start_button_label, btn_label_buf);
    lv_obj_set_style_text_color(start_button_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(start_button_label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_align(start_button_label, LV_ALIGN_CENTER);

    stop_button = lv_btn_create(control_buttons_cont);
    lv_obj_set_size(stop_button, 200, 90);
    lv_obj_add_style(stop_button, &btn_style,    LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(stop_button, &btn_pr_style, LV_PART_MAIN | LV_STATE_PRESSED);

    //////// DEFAULT STATE STYLES ////////
    lv_obj_set_style_bg_color(stop_button,       lv_color_hex(0xF26B27), LV_PART_MAIN | LV_STATE_DEFAULT);        // фон
    lv_obj_set_style_border_color(stop_button,   lv_color_hex(0xF26B27), LV_PART_MAIN | LV_STATE_DEFAULT);  // колір рамки
    lv_obj_set_style_shadow_color(stop_button,   lv_color_hex(0xF26B27), LV_PART_MAIN | LV_STATE_DEFAULT);  // glow-ефект

    //////// PRESSED STATE STYLES ////////
    lv_obj_set_style_bg_color(stop_button,    lv_color_hex(0xF26B27), LV_PART_MAIN | LV_STATE_PRESSED); // темніший фон
    lv_obj_set_style_bg_opa(stop_button,      LV_OPA_30, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_border_color(stop_button, lv_color_hex(0xF26B27), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_border_width(stop_button, 2, LV_PART_MAIN | LV_STATE_PRESSED);

    lv_obj_add_event_cb(stop_button, stop_button_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_set_grid_cell(stop_button,
        LV_GRID_ALIGN_STRETCH, 1, 1,  // кол.1, span=1
        LV_GRID_ALIGN_CENTER, 0, 1    // ряд.0, span=1
    );

    stop_button_label = lv_label_create(stop_button);
    lv_snprintf(btn_label_buf, sizeof(btn_label_buf) - 1, " %s  STOP", LV_SYMBOL_STOP);
    lv_label_set_text(stop_button_label, btn_label_buf);
    lv_obj_set_style_text_color(stop_button_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(stop_button_label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_align(stop_button_label, LV_ALIGN_CENTER);

    immediate_stop_button = lv_btn_create(control_buttons_cont);
    lv_obj_set_size(immediate_stop_button, 200, 90);
    lv_obj_add_style(immediate_stop_button, &btn_style,    LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(immediate_stop_button, &btn_pr_style, LV_PART_MAIN | LV_STATE_PRESSED);

    //////// DEFAULT STATE STYLES ////////
    lv_obj_set_style_bg_color(immediate_stop_button,       lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);        // фон
    lv_obj_set_style_border_color(immediate_stop_button,   lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);  // колір рамки
    lv_obj_set_style_shadow_color(immediate_stop_button,   lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);  // glow-ефект

    //////// PRESSED STATE STYLES ////////
    lv_obj_set_style_bg_color(immediate_stop_button,    lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_PRESSED); // темніший фон
    lv_obj_set_style_bg_opa(immediate_stop_button,      LV_OPA_30, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_border_color(immediate_stop_button, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_border_width(immediate_stop_button, 2, LV_PART_MAIN | LV_STATE_PRESSED);

    lv_obj_add_event_cb(immediate_stop_button, immediate_stop_button_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_set_grid_cell(immediate_stop_button,
        LV_GRID_ALIGN_STRETCH, 2, 1,  // кол.0, span=3
        LV_GRID_ALIGN_CENTER, 0, 1    // ряд.1, span=1
    );

    immediate_stop_button_label = lv_label_create(immediate_stop_button);
    lv_snprintf(btn_label_buf, sizeof(btn_label_buf) - 1, " %s  ABORT", LV_SYMBOL_WARNING);
    lv_label_set_text(immediate_stop_button_label, btn_label_buf);
    lv_obj_set_style_text_color(immediate_stop_button_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(immediate_stop_button_label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_align(immediate_stop_button_label, LV_ALIGN_CENTER);


    manual_acceleration_button = lv_btn_create(control_buttons_cont);
    lv_obj_set_size(manual_acceleration_button, 200, 90);
    lv_obj_add_style(manual_acceleration_button, &btn_style,    LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(manual_acceleration_button, &btn_pr_style, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_event_cb(manual_acceleration_button, accelerate_button_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_set_grid_cell(manual_acceleration_button,
        LV_GRID_ALIGN_STRETCH, 0, 1,  // кол.0
        LV_GRID_ALIGN_CENTER, 1, 1    // ряд.2
    );

    manual_acceleration_button_label = lv_label_create(manual_acceleration_button);
    lv_snprintf(btn_label_buf, sizeof(btn_label_buf) - 1, " %s  ACCEL", LV_SYMBOL_UP);
    lv_label_set_text(manual_acceleration_button_label, btn_label_buf);
    lv_obj_set_style_text_color(manual_acceleration_button_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(manual_acceleration_button_label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_align(manual_acceleration_button_label, LV_ALIGN_CENTER);

    manual_deceleration_button = lv_btn_create(control_buttons_cont);
    lv_obj_set_size(manual_deceleration_button, 200, 90);
    lv_obj_add_style(manual_deceleration_button, &btn_style,    LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(manual_deceleration_button, &btn_pr_style, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_event_cb(manual_deceleration_button, decelerate_button_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_set_grid_cell(manual_deceleration_button,
        LV_GRID_ALIGN_STRETCH, 1, 1,  // кол.1
        LV_GRID_ALIGN_CENTER, 1, 1    // ряд.2
    );

    manual_deceleration_button_label = lv_label_create(manual_deceleration_button);
    lv_snprintf(btn_label_buf, sizeof(btn_label_buf) - 1, " %s  DECEL", LV_SYMBOL_DOWN);
    lv_label_set_text(manual_deceleration_button_label, btn_label_buf);
    lv_obj_set_style_text_color(manual_deceleration_button_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(manual_deceleration_button_label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_align(manual_deceleration_button_label, LV_ALIGN_CENTER);

    home_point_button = lv_btn_create(control_buttons_cont);
    lv_obj_set_size(home_point_button, 200, 90);
    lv_obj_add_style(home_point_button, &btn_style,    LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(home_point_button, &btn_pr_style, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_event_cb(home_point_button, home_point_button_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_set_grid_cell(home_point_button,
        LV_GRID_ALIGN_STRETCH, 2, 1,  // кол.2
        LV_GRID_ALIGN_CENTER, 1, 1    // ряд.2
    );

    home_point_button_label = lv_label_create(home_point_button);
    lv_snprintf(btn_label_buf, sizeof(btn_label_buf) - 1, " %s  HOME", LV_SYMBOL_HOME);
    lv_label_set_text(home_point_button_label, btn_label_buf);
    lv_obj_set_style_text_color(home_point_button_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(home_point_button_label, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_align(home_point_button_label, LV_ALIGN_CENTER);

    telemetry_cont = lv_obj_create(cont_main_right);
    lv_obj_set_size(telemetry_cont, 230, LV_SIZE_CONTENT);
    lv_obj_set_layout(telemetry_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(telemetry_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(telemetry_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(telemetry_cont, 20, 0);
    lv_obj_set_style_bg_opa(telemetry_cont, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(telemetry_cont, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);

    //////////// TELEMETRY //////////////

    // lv_obj_t * telemetry_header_label = lv_label_create(telemetry_cont);
    // lv_label_set_text(telemetry_header_label, "-- Telemetry --");
    // lv_obj_set_size(telemetry_header_label, LV_PCT(100), LV_SIZE_CONTENT);
    // lv_obj_set_align(telemetry_header_label, LV_ALIGN_CENTER);

    current_winding_length_label = lv_label_create(telemetry_cont);
    lv_label_set_text(current_winding_length_label, "Current Length: N/A");

    used_cable_total_length_label = lv_label_create(telemetry_cont);
    lv_label_set_text(used_cable_total_length_label, "Used Length: N/A");

    current_speed_label = lv_label_create(telemetry_cont);
    lv_label_set_text(current_speed_label, "Rotation Speed: N/A");

    operating_time_label = lv_label_create(telemetry_cont);
    lv_label_set_text(operating_time_label, "Operating Time: N/A");

    lv_obj_t *reset_btn = lv_btn_create(telemetry_cont);
    lv_obj_set_size(reset_btn, 180, 40);
    lv_obj_add_style(reset_btn, &btn_style, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(reset_btn, &btn_pr_style, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_event_cb(reset_btn, reset_button_clicked, LV_EVENT_CLICKED, NULL);

    lv_obj_t *reset_label = lv_label_create(reset_btn);
    lv_label_set_text(reset_label, "RESET");
    lv_obj_center(reset_label);
    
    lv_obj_t *kb = lv_keyboard_create(lv_layer_top());
    lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_USER_1);
    lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_USER_1, custom_map, custom_styles);
    lv_obj_set_size(kb, LV_PCT(100), LV_PCT(50));
    lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(kb, kb_event_handler, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(rotate_per_sec_entry, ta_focus_handler, LV_EVENT_CLICKED, kb);
    lv_obj_add_event_cb(carriage_movement_entry, ta_focus_handler, LV_EVENT_CLICKED, kb);
    lv_obj_add_event_cb(general_winding_length_entry, ta_focus_handler, LV_EVENT_CLICKED, kb);
    lv_obj_add_event_cb(revers_conical_layers_entry, ta_focus_handler, LV_EVENT_CLICKED, kb);
    lv_obj_add_event_cb(revers_conical_delay_mm_entry, ta_focus_handler, LV_EVENT_CLICKED, kb);
}