#define NUM_STATES 5
#define NUM_EVENTS 5

// definicao dos possiveis eventos gerados pelos estados da FSM
typedef enum event_ {int_connect, server, question, send_button, send_time} event;
// definicao das funcoes que implementam o comportamento de cada estado
event int_connect_state(void);
event mqqt_state(void);
event time_pass_state(void);
event btn_pressed_state(void);
event send_data_state(void);
event back_state(void);
// array de ponteiros para as funcoes dos estados
event (* state_functions[])(void) = {int_connect_state, time_pass_state, btn_pressed_state, send_data_state, back_state };
// definicao dos nomes dos estados
typedef enum state_ {internet, mqqt, question, send_button, send_time} state;
// estrutura que define as transicoes dos estados
state state_transitions[NUM_STATES][NUM_EVENTS] = {{internet, mqqt, internet, internet, internet},
                                                   {mqqt, mqqt, question, mqqt, mqqt}
                                                   {question, question, question, send_button, send_time}
                                                   {send_button, send_button, send_button, internet, send_button}
                                                   {send_time, send_time, send_time, send_time, internet}};
// definicao dos estados inicial e final
#define EXIT_STATE mqqt
#define ENTRY_STATE internet

// funcao que implementa a ransicao de estados
state lookup_transitions(state cur_state, event cur_evt) {
  return state_transitions[cur_state][cur_evt];
}
