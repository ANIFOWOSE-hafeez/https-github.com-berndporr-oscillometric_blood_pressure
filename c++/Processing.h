#ifndef OBP_PROCESSING_H
#define OBP_PROCESSING_H

#include <vector>

#include <comedilib.h>
#include <Iir.h>

#include "CppThread.h"
#include "datarecord.h"
#include "ISubject.h"


#define IIRORDER 6

#define COMEDI_SUB_DEVICE  0
#define COMEDI_RANGE_ID    0   /* +/- 1.325V  for sigma device*/
#define COMEDI_NUM_CHANNEL 1
#define SAMPLING_RATE 1000 // 1kHz





class IEventListener { // "Observed" - Listener needs to implement these functions
public:
    virtual void eNewData(double pData, double oData) {};
    virtual void eSwitchScreen(Screen eScreen) {}; //TBD

};

class Processing : public CppThread, public ISubject{


public:
    Processing();
    ~Processing() override;

    bool bSaveToFile();
    void stopThread();

    void startMeasurement();
    void stopMeasurement();
private:
    IEventListener* cb = NULL;
    void run() override;
    std::vector<double> pData;// = std::vector<double>(122880);
    std::vector<double> oData;// = std::vector<double>(122880);

    Iir::Butterworth::LowPass<IIRORDER> *iirLP;
    Iir::Butterworth::HighPass<IIRORDER> *iirHP;

    Datarecord *record;
    bool bRunning; // process is running and displaying data on screen, but not necessary recording/measuring blood pressure it.
    bool bMeasuring;


//TODO: move to comedi class?

    int adChannel;
    /**
   * file descriptor for /dev/comedi0
   **/
    comedi_cmd comediCommand;
    comedi_t *dev;
    size_t readSize;
    bool sigmaBoard;//TODO: warning, if not sigma board
    lsampl_t maxdata;
    comedi_range *crange;
    double sampling_rate;

    int numChannels;
    unsigned *chanlist;

    void addSample(double sample);

    static QString getFilename();
};


#endif //OBP_PROCESSING_H
