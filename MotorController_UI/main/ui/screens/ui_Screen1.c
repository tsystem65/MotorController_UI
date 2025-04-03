#include "../ui.h"

// static void start_button_event_handler(lv_event_t *e) {
//     send_command("Start");
// }

// static void stop_button_event_handler(lv_event_t *e) {
//     send_command("Stop");
// }

// static void accelerate_button_event_handler(lv_event_t *e) {
//     send_command("Accelerate");
// }

// static void immediate_stop_button_event_handler(lv_event_t *e) {
//     send_command("Immediate Stop");
// }

// static void decelerate_button_event_handler(lv_event_t *e) {
//     send_command("Decelerate");
// }

// static void home_point_button_event_handler(lv_event_t *e) {
//     send_command("Home Point");
// }
// static lv_obj_t * kb;

// static void ta_event_cb(lv_event_t * e)
// {
//     lv_event_code_t code = lv_event_get_code(e);
//     lv_obj_t * ta = lv_event_get_target(e);
//     if(code == LV_EVENT_CLICKED || code == LV_EVENT_FOCUSED) {
//         /*Focus on the clicked text area*/
//         if(kb != NULL) lv_keyboard_set_textarea(kb, ta);
//     }

//     lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN); // Показуємо клавіатуру
//     lv_obj_add_state(ta, LV_STATE_FOCUSED); // Фокус на полі
// }

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
    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);
    // lv_obj_set_layout(ui_Screen1, LV_LAYOUT_FLEX);
    // lv_obj_set_flex_flow(ui_Screen1, LV_FLEX_FLOW_COLUMN);
    // lv_obj_set_flex_align(ui_Screen1, LV_ALIGN_TOP_MID, LV_ALIGN_CENTER, LV_ALIGN_DEFAULT);
    // lv_obj_set_style_pad_all(ui_Screen1, 0, 0); 

    lv_obj_t * main_screen_cont = lv_obj_create(ui_Screen1);
    lv_obj_set_size(main_screen_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    //lv_obj_clear_flag(main_screen_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_layout(main_screen_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(main_screen_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(main_screen_cont, LV_ALIGN_TOP_MID, LV_ALIGN_CENTER, LV_ALIGN_DEFAULT);
    lv_obj_set_style_pad_all(main_screen_cont, 0, 0); 

    lv_obj_t * operation_params_cont = lv_obj_create(main_screen_cont);
    lv_obj_set_size(operation_params_cont, 800, LV_SIZE_CONTENT);
    lv_obj_set_align(operation_params_cont, LV_ALIGN_TOP_MID);
    lv_obj_set_layout(operation_params_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(operation_params_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(operation_params_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(operation_params_cont, 15, 0);
    //lv_obj_set_style_bg_opa(operation_params_cont, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_border_opa(operation_params_cont, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_clear_flag(operation_params_cont, LV_OBJ_FLAG_SCROLLABLE);
    
    operation_params_input_cont = lv_obj_create(operation_params_cont);
    lv_obj_set_size(operation_params_input_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    // Визначаємо колонки (лейбл: 180px, поле вводу: 220px) і рядки (по 50px)
    static lv_coord_t operation_params_input_cont_col_dsc[] = {180, 220, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t operation_params_input_cont_row_dsc[] = {50, 50, 50, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(operation_params_input_cont, operation_params_input_cont_col_dsc, operation_params_input_cont_row_dsc);

    // 2. Створення елементів безпосередньо в сітці
    // Перший рядок - Rotation Speed
    rotate_per_sec_label = lv_label_create(operation_params_input_cont);
    lv_label_set_text(rotate_per_sec_label, "Rotation Speed (rps):");
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

    // Другий рядок - Winding Step
    carriage_movement_label = lv_label_create(operation_params_input_cont);
    lv_label_set_text(carriage_movement_label, "Winding Step (mm):");
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

    // Третій рядок - Total Length
    general_winding_length_label = lv_label_create(operation_params_input_cont);
    lv_label_set_text(general_winding_length_label, "Total Length (m):");
    lv_obj_set_grid_cell(general_winding_length_label,
        LV_GRID_ALIGN_START, 0, 1,
        LV_GRID_ALIGN_CENTER, 2, 1
    );

    general_winding_length_entry = lv_textarea_create(operation_params_input_cont);
    lv_textarea_set_one_line(general_winding_length_entry, true);  // Виправлено: було rotate_per_sec_entry
    lv_obj_set_grid_cell(general_winding_length_entry,
        LV_GRID_ALIGN_STRETCH, 1, 1,
        LV_GRID_ALIGN_CENTER, 2, 1
    );

    lv_obj_t * operation_params_cont_separator = lv_line_create(operation_params_cont);
    static lv_point_t operation_params_cont_separator_points[] = {{0, 0}, {0, 200}};  // Вертикальна лінія 100px
    lv_line_set_points(operation_params_cont_separator, operation_params_cont_separator_points, 2);
    lv_obj_set_style_line_width(operation_params_cont_separator, 1, 0);

    operation_mode_cont = lv_obj_create(operation_params_cont);
    lv_obj_set_size(operation_mode_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_layout(operation_mode_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(operation_mode_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(operation_mode_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(operation_mode_cont, 40, 0);

    lv_obj_t * operation_mode_header_label = lv_label_create(operation_mode_cont);
    lv_label_set_text(operation_mode_header_label, "-- Operation Modes --");

    grinding_mode_checkbox = lv_checkbox_create(operation_mode_cont);
    lv_checkbox_set_text(grinding_mode_checkbox, " Grinding mode");

    conical_winding_checkbox = lv_checkbox_create(operation_mode_cont);
    lv_checkbox_set_text(conical_winding_checkbox, " Conical winding");

    buttons_telemetry_cont = lv_obj_create(main_screen_cont);
    lv_obj_set_size(buttons_telemetry_cont, 800, LV_SIZE_CONTENT);
    lv_obj_set_align(buttons_telemetry_cont, LV_ALIGN_TOP_MID);
    lv_obj_set_layout(buttons_telemetry_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(buttons_telemetry_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(buttons_telemetry_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(buttons_telemetry_cont, 15, 0);
    //lv_obj_set_style_bg_opa(buttons_telemetry_cont, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    //lv_obj_set_style_border_opa(buttons_telemetry_cont, LV_OPA_TRANSP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(buttons_telemetry_cont, LV_OBJ_FLAG_SCROLLABLE);

    control_buttons_cont = lv_obj_create(buttons_telemetry_cont);
    lv_obj_set_size(control_buttons_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    static lv_coord_t control_buttons_cont_col_dsc[] = {200, 200, LV_GRID_TEMPLATE_LAST};
    static lv_coord_t control_buttons_cont_row_dsc[] = {50, 50, 50, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(control_buttons_cont, control_buttons_cont_col_dsc, control_buttons_cont_row_dsc);

    start_button = lv_btn_create(control_buttons_cont);
    lv_obj_set_size(start_button, 200, 50);
    lv_obj_set_grid_cell(start_button, 
        LV_GRID_ALIGN_START, 0, 1,  // Колонка 0, ширина 1
        LV_GRID_ALIGN_CENTER, 0, 1   // Рядок 0, висота 1
    );

    start_button_label = lv_label_create(start_button);
    lv_label_set_text(start_button_label, "Start");
    lv_obj_set_align(start_button_label, LV_ALIGN_CENTER);

    stop_button = lv_btn_create(control_buttons_cont);
    lv_obj_set_size(stop_button, 200, 50);
    lv_obj_set_grid_cell(stop_button, 
        LV_GRID_ALIGN_START, 1, 1,
        LV_GRID_ALIGN_CENTER, 0, 1
    );

    stop_button_label = lv_label_create(stop_button);
    lv_label_set_text(stop_button_label, "Stop");
    lv_obj_set_align(stop_button_label, LV_ALIGN_CENTER);

    manual_acceleration_button = lv_btn_create(control_buttons_cont);
    lv_obj_set_size(manual_acceleration_button, 200, 50);
    lv_obj_set_grid_cell(manual_acceleration_button, 
        LV_GRID_ALIGN_START, 0, 1,
        LV_GRID_ALIGN_CENTER, 1, 1
    );

    manual_acceleration_button_label = lv_label_create(manual_acceleration_button);
    lv_label_set_text(manual_acceleration_button_label, "Accelerate");
    lv_obj_set_align(manual_acceleration_button_label, LV_ALIGN_CENTER);

    manual_deceleration_button = lv_btn_create(control_buttons_cont);
    lv_obj_set_size(manual_deceleration_button, 200, 50);
    lv_obj_set_grid_cell(manual_deceleration_button, 
        LV_GRID_ALIGN_START, 0, 1,
        LV_GRID_ALIGN_CENTER, 2, 1
    );

    manual_deceleration_button_label = lv_label_create(manual_deceleration_button);
    lv_label_set_text(manual_deceleration_button_label, "Decelerate");
    lv_obj_set_align(manual_deceleration_button_label, LV_ALIGN_CENTER);

    immediate_stop_button = lv_btn_create(control_buttons_cont);
    lv_obj_set_size(immediate_stop_button, 200, 50);
    lv_obj_set_grid_cell(immediate_stop_button, 
        LV_GRID_ALIGN_START, 1, 1,
        LV_GRID_ALIGN_CENTER, 1, 1
    );

    immediate_stop_button_label = lv_label_create(immediate_stop_button);
    lv_label_set_text(immediate_stop_button_label, "Immediate Stop");
    lv_obj_set_align(immediate_stop_button_label, LV_ALIGN_CENTER);

    home_point_button = lv_btn_create(control_buttons_cont);
    lv_obj_set_size(home_point_button, 200, 50);
    lv_obj_set_grid_cell(home_point_button, 
        LV_GRID_ALIGN_START, 1, 1,
        LV_GRID_ALIGN_CENTER, 2, 1
    );

    home_point_button_label = lv_label_create(home_point_button);
    lv_label_set_text(home_point_button_label, "Home Point");
    lv_obj_set_align(home_point_button_label, LV_ALIGN_CENTER);

    telemetry_cont = lv_obj_create(buttons_telemetry_cont);
    lv_obj_set_size(telemetry_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_layout(telemetry_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(telemetry_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(telemetry_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(telemetry_cont, 20, 0);

    current_winding_length_label = lv_label_create(telemetry_cont);
    lv_label_set_text(current_winding_length_label, "Current Length: 10021,22 m");

    used_cable_total_length_label = lv_label_create(telemetry_cont);
    lv_label_set_text(used_cable_total_length_label, "Used Length: 22154,22 m");

    current_speed_label = lv_label_create(telemetry_cont);
    lv_label_set_text(current_speed_label, "Speed: 4.71 rps");

    operating_time_label = lv_label_create(telemetry_cont);
    lv_label_set_text(operating_time_label, "Operating Time: 01:32:33");
    
    /*Create a keyboard*/
    // kb = lv_keyboard_create(ui_Screen1);
    // lv_obj_set_size(kb,  LV_HOR_RES, LV_VER_RES / 2);

    // lv_keyboard_set_textarea(kb, rotate_per_sec_entry);
    // lv_obj_add_event_cb(rotate_per_sec_entry, ta_event_cb, LV_EVENT_ALL, NULL);

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

    //lv_obj_add_state(rotate_per_sec_entry, LV_STATE_FOCUSED);
}