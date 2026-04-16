#ifndef __OPENTSS_H__
#define __OPENTSS_H__
#include <Arduino.h>

#define TSS_VERSION "1.0.0"

/** Class that returns callbacks at regular intervals */
class OpenTSS{
  public:
    static constexpr int8_t DEFAULT_MAX_TRIG = 8; // maximum number of triggers

    /// @brief  Constructor with optional parameter for maximum number of triggers
    /// @param _maxTrig 
    OpenTSS(int8_t _maxTrig = DEFAULT_MAX_TRIG) {
        setup(_maxTrig);
    };

    ~OpenTSS(){
      if(m_trigWorkArr!=NULL){
        free(m_trigWorkArr);
        m_trigWorkArr=NULL;
      }
    };

    [[deprecated("Setup() has already been called from the constructor")]]
    void Setup(int8_t _maxTrig=8){
      setup(_maxTrig);
    }

    /// @brief  Add a function to be called at regular intervals
    /// @param _pFunc Pointer to the function to be called
    /// @param _trigTime Interval between callbacks in milliseconds
    /// @return Trigger ID on success, -1 on failure
    int8_t AddTrig(void (*_pFunc)(uint32_t), uint32_t _trigTime){
        for (int8_t trigId = 0; trigId < m_trigNum; ++trigId) {
            if (m_trigWorkArr[trigId].func == nullptr) {
                m_trigWorkArr[trigId].func = _pFunc;
                m_trigWorkArr[trigId].trigTime = _trigTime;
                m_trigWorkArr[trigId].timer = 0;
                return trigId;
            }
        }
        return -1; // 失敗時
    }

    /// @brief Remove a function from the trigger list
    /// @param _trigId ID of the trigger to remove
    /// @return True on success, false otherwise
    bool RemoveTrig(int8_t _trigId){
        if (_trigId < 0 || _trigId >= m_trigNum) {
            return false; // 範囲外チェック
        }
        if (m_trigWorkArr[_trigId].func != nullptr) {
            m_trigWorkArr[_trigId].reset();
            return true;
        }
        return false;
    }

    /// @brief Call this in loop()
    /// @return Time elapsed since the last call in milliseconds
    uint32_t Update(){
      uint32_t nowMillis = millis();
      uint32_t deltaMillis = nowMillis - m_currentMillis;
      m_currentMillis = nowMillis;
      return Update(deltaMillis);
    };

    /// @brief Call this if you need to update TSS with a different time interval
    /// @param _deltaMillis Time elapsed since the last call in milliseconds
    /// @return The same time interval passed as input
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

    /// @brief Get the number of available trigger slots
    /// @return Number of remaining triggers
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

    /// @brief Setup timer system.
    /// @param _maxTrig Maximum number of timer triggers (default: 8, max:127)    */
    void setup(int8_t _maxTrig = DEFAULT_MAX_TRIG){
        m_trigNum = (_maxTrig > 0) ? _maxTrig : 1;
        m_trigWorkArr = (TrigWork*)malloc(sizeof(TrigWork) * m_trigNum);
        for (int i = 0; i < m_trigNum; ++i) {
            m_trigWorkArr[i].reset();
        }
        m_currentMillis = millis();
    }

    TrigWork* m_trigWorkArr;
    int8_t m_trigNum;
    uint32_t m_currentMillis;
};
#endif // __OPENTSS_H__