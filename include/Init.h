#include "stm32f10x.h"
#define ARR     144
void Init(void);

typedef enum
{
  ATTACK = 0,
  DECAY,
  SUSTAIN,
  RELEASE
}TypeDefADSRState;			
typedef struct
{
  uint16_t attack_time;         //time
  uint16_t decay_time;
  uint16_t sustain_time;
  uint16_t release_time;
  
  uint16_t *attack_data;        //data        
  uint16_t *decay_data;
  uint16_t *sustain_data;
  uint16_t *release_data;
  
  uint16_t attack_data_size;
  uint16_t decay_data_size;
  uint16_t sustain_data_size;
  uint16_t release_data_size;
  TypeDefADSRState state;
  uint8_t flag;
}TypeDefADSR;


