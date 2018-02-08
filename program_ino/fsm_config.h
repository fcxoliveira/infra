#define NUM_STATES 5
#define NUM_EVENTS 3

// definicao dos possiveis eventos gerados pelos estados da FSM
typedef enum event_ {first, next, back} event;
// definicao das funcoes que implementam o comportamento de cada estado
event int_connect_state(void);
event mqqt_state(void);
event _question_state(void);
event send_button_state(void);
event send_time_state(void);
// array de ponteiros para as funcoes dos estados
event (* state_functions[])(void) = {int_connect_state, mqqt_state, _question_state, send_button_state, send_time_state };
// definicao dos nomes dos estados
typedef enum state_ {internet, mqqt, _question, send_button, send_time, end} state;
// estrutura que define as transicoes dos estados
state state_transitions[NUM_STATES][NUM_EVENTS] = {{internet, mqqt, internet},
                                                   {mqqt, _question, mqqt},
                                                   {send_button, send_time, _question},
                                                   {send_button, send_button, _question},
                                                   {send_time, send_time, _question}};
// definicao dos estados inicial e final
#define EXIT_STATE end
#define ENTRY_STATE internet

// funcao que implementa a ransicao de estados
state lookup_transitions(state cur_state, event cur_evt) {
  return state_transitions[cur_state][cur_evt];
}
