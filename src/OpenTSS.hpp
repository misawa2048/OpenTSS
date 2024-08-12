#ifndef __OPENTSS_H__
#define __OPENTSS_H__
#include <Arduino.h>

/** Class that returns callbacks at regular intervals */
class OpenTSS{
    public:
    OpenTSS(){
      m_trigWorkArr=NULL;
    };
    ~OpenTSS(){
      if(m_trigWorkArr!=NULL){
        free(m_trigWorkArr);
        m_trigWorkArr=NULL;
      }
    };

    /** Setup timer system.
     *  _maxTrig:Maximum number of timer triggers (default: 8, max:127)    */
    void Setup(int8_t _maxTrig=8){
      m_trigNum = (_maxTrig>0)?_maxTrig:1;
      m_trigWorkArr = (TrigWork*)malloc(sizeof(TrigWork)*m_trigNum);
      for(int i=0;i<m_trigNum;++i){
        m_trigWorkArr[i].reset();
      }
      m_currentMillis = millis();
    }

    /** Add a func(uint32_t) that is called at regular intervals
     * uint32_t: Interval between callbacks[msec]
     * return: sucsess=triggerID, -1 on failure    */
    int8_t AddTrig(void (*_pFunc)(uint32_t), uint32_t _trigTime){
      for(int8_t trigId=0;trigId<m_trigNum;++trigId){
        if(m_trigWorkArr[trigId].func==NULL){
          m_trigWorkArr[trigId].func = _pFunc;
          m_trigWorkArr[trigId].trigTime = _trigTime;
          m_trigWorkArr[trigId].timer = 0;
          return trigId;
        }
      }
      return -1;
    }

    /** Remove function called at regular intervals
     * _trigId:triggerID
     * return: true when success   */
     bool RemoveTrig(int8_t _trigId){
      if(m_trigWorkArr[_trigId].func!=NULL){
        m_trigWorkArr[_trigId].reset();
        return true;
      }
      return false;
    }

    /** Call this in loop()
     * return: Time since last call [ms] */
    uint32_t Update(){
      uint32_t nowMillis = millis();
      uint32_t deltaMillis = nowMillis - m_currentMillis;
      m_currentMillis = nowMillis;
      return Update(deltaMillis);
    };

    /** Call this if you need to update TSS with a different time interval
     * _deltaMillis: Time since last call [ms] */
    uint32_t Update(uint32_t _deltaMillis){
      for(int8_t i=0;i<m_trigNum;++i){
        if(m_trigWorkArr[i].func!=NULL){
          m_trigWorkArr[i].timer += _deltaMillis;
          if(m_trigWorkArr[i].timer >= m_trigWorkArr[i].trigTime){
            m_trigWorkArr[i].func(m_trigWorkArr[i].timer);
            m_trigWorkArr[i].timer -= m_trigWorkArr[i].trigTime;
          }
        }
      }
      return _deltaMillis;
    };

    /** Get the number of remaining triggers available
     * return : number of triggers */
    int8_t GetRemainTrigNum(){
        int8_t cnt = 0;
        for(int8_t i=0;i<m_trigNum;++i)
          if(m_trigWorkArr[i].func==NULL)
            cnt++;
        return cnt;
    }
    private:
    struct TrigWork{
      void (*func)(uint32_t);
      uint32_t trigTime;
      uint32_t timer;
      void reset(){
        func = NULL;
        trigTime = 0;
        timer = 0;
      }
    } _TrigWork;

    TrigWork* m_trigWorkArr;
    int8_t m_trigNum;
    uint32_t m_currentMillis;
};
#endif // __OPENTSS_H__