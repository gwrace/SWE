/*! \file
 * \brief     SID simulation implementation
 * \author    cpr
 * \date      2024
 * \copyright h_da fbeit
 */

#include "SID.h"

#include <iostream>
#include <utility>

#if defined(REAL_SID_ATTACHED)
    #include <sstream>
    #include <cstring>
    #include <stdlib.h>
    #include <sys/ioctl.h>
    #include <sys/fcntl.h>
    #include <sys/termios.h>
    #include <unistd.h>
    #include <thread>
    #include <string>
#endif  // defined(REAL_SID_ATTACHED)


namespace sid_types
{

using std::map;
using std::size_t;
using std::pair;
using std::array;
using namespace std::chrono;
using std::cout;
using std::endl;
#if defined(REAL_SID_ATTACHED)
    using std::this_thread::sleep_for;
    using std::chrono::milliseconds;
    using std::chrono::microseconds;
    using std::stringstream;
    using std::flush;
    using std::string;
#endif  // defined(REAL_SID_ATTACHED)

const PinNumber SID::NRES = 5;
const PinNumber SID::NCS  = 8;
const array<PinNumber, 5> SID::A = { 9, 10, 11, 12, 13 };
const array<PinNumber, 8> SID::D = { 15, 16, 17, 18, 19, 20, 21, 22 };

const map<VoiceIdx, Address> SID::V_FREQ_LO = {
    { V1,  0 },
    { V2,  7 },
    { V3, 14 } };
const map<VoiceIdx, Address> SID::V_FREQ_HI = {
    { V1,  1 },
    { V2,  8 },
    { V3, 15 } };
const map<VoiceIdx, Address> SID::V_PW_LO = {
    { V1,  2 },
    { V2,  9 },
    { V3, 16 } };
const map<VoiceIdx, Address> SID::V_PW_HI = {
    { V1,  3 },
    { V2, 10 },
    { V3, 17 } };
const map<VoiceIdx, Address> SID::V_CONTROL = {
    { V1,  4 },
    { V2, 11 },
    { V3, 18 } };
const map<VoiceIdx, Address> SID::V_ATTACK_DECAY = {
    { V1,  5 },
    { V2, 12 },
    { V3, 19 } };
const map<VoiceIdx, Address> SID::V_SUSTAIN_RELEASE = {
    { V1,  6 },
    { V2, 13 },
    { V3, 20 } };
const Address SID::FC_LO    = 21;
const Address SID::FC_HI    = 22;
const Address SID::RES_FILT = 23;
const Address SID::MODE_VOL = 24;


SID::SID()
{
    #if defined(REAL_SID_ATTACHED)
        if(!realSID_.connected())
        {
            cout << "SID: Warning: RealSID not connected." << endl;
        }
    #endif
    startTime_ = steady_clock::now();
    for(VoiceIdx v = V1; v<N_VOICES; v++)
    {
        Voice& voice = voices_[v];
        voiceSetFcts_[V_FREQ_LO.at(v)]
            = [&voice](b8 a_data){ return voice.setFreqLo(a_data); };
        voiceSetFcts_[V_FREQ_HI.at(v)]
            = [&voice](b8 a_data){ return voice.setFreqHi(a_data); };
        voiceSetFcts_[V_PW_LO.at(v)]
            = [&voice](b8 a_data){ return voice.setPWLo(a_data); };
        voiceSetFcts_[V_PW_HI.at(v)]
            = [&voice](b8 a_data){ return voice.setPWHi(a_data); };
        voiceSetFcts_[V_CONTROL.at(v)]
            = [&voice](b8 a_data){ return voice.setControl(a_data); };
        voiceSetFcts_[V_ATTACK_DECAY.at(v)]
            = [&voice](b8 a_data){ return voice.setAttackDecay(a_data); };
        voiceSetFcts_[V_SUSTAIN_RELEASE.at(v)]
            = [&voice](b8 a_data){ return voice.setSustainRelease(a_data); };
        filterBanks_[v];
    }

    sampleCtr_ = 0;

    reset(false);
}

SID::~SID()
{
    if(sampleCtr_ > SAMPLERATE_HZ/4)
    {
        float took = (duration_cast<milliseconds>(steady_clock::now()
                     - startTime_).count()) / 1000.0;
        cout << "Simulation took " << took << " seconds. You seem to have a ";
        float realtime = (float)sampleCtr_/PHI2;
        if(took <= realtime/2)
            cout << "really fast";
        else if(took <= realtime)
            cout << "rather fast";
        else if(took <= 1.5*realtime)
            cout << "fast enough";
        else if(took <= 2.0*realtime)
            cout << "quite slow";
        else
            cout << "really slow";
        cout << " machine!" << endl;
    }

    #if defined(REAL_SID_ATTACHED)
        realSID_.play();
    #endif
}

void SID::writePin(PinNumber a_pinNumber, LogicLevel a_value)
{
    pins_[a_pinNumber-1] = a_value;
    return;
}

void SID::reset(bool a_verbose)
{
    if(a_verbose)
        cout << "SID has been reset." << endl;
    pins_.reset();
    volume_ = 0.0;
    filterCutoffFreqLo_ = 0;
    filterCutoffFreqHi_ = 0;
    for(pair<const VoiceIdx, Voice>& voice : voices_)
    {
        voice.second.reset();
        VoiceIdx v = voice.first;
        filterBanks_[v].reset();
        voicePassFilter_[v] = false;
    }
    filterEnabled_[LP] = false;
    filterEnabled_[BP] = false;
    filterEnabled_[HP] = false;

    return;
}

void SID::onNRES()
{
    #if defined (REAL_SID_ATTACHED)
        realSID_.send({sampleCtr_, 0b01000000, 0});
    #endif

    static int resetCtr = 1;

    if(resetCtr == 10)
    {
        reset(true);
        resetCtr = 0;
    }
    resetCtr++;

    return;
}

float SID::onCLK()
{
    sampleCtr_++;

    if(!pins_[NRES-1])
    {
        onNRES();
        return 0.0;
    }

    if(!pins_[NCS-1])
    {
        Address addr =
                  (pins_[A[0]-1] << 0)
                + (pins_[A[1]-1] << 1)
                + (pins_[A[2]-1] << 2)
                + (pins_[A[3]-1] << 3)
                + (pins_[A[4]-1] << 4);

        b8 data =
                  (pins_[D[0]-1] << 0)
                + (pins_[D[1]-1] << 1)
                + (pins_[D[2]-1] << 2)
                + (pins_[D[3]-1] << 3)
                + (pins_[D[4]-1] << 4)
                + (pins_[D[5]-1] << 5)
                + (pins_[D[6]-1] << 6)
                + (pins_[D[7]-1] << 7);

        #if defined (REAL_SID_ATTACHED)
            realSID_.send({sampleCtr_, static_cast<u8>(0b00100000 | addr), data});
        #endif

        switch(addr)
        {
        case FC_LO:
        {
            filterCutoffFreqLo_ = data & 0b00000111;
            b16 filterCutoffFreq
                = ((b16)filterCutoffFreqHi_ << 3) | filterCutoffFreqLo_;
            double fco = (30.0 + filterCutoffFreq*5.8);
            filterBanks_[V1].setCutoffFreq(fco);
            filterBanks_[V2].setCutoffFreq(fco);
            filterBanks_[V3].setCutoffFreq(fco);
            break;
        }

        case FC_HI:
        {
            filterCutoffFreqHi_ = data;
            b16 filterCutoffFreq
                = ((b16)filterCutoffFreqHi_ << 3) | filterCutoffFreqLo_;
            double fco = (30.0 + filterCutoffFreq*5.8);
            filterBanks_[V1].setCutoffFreq(fco);
            filterBanks_[V2].setCutoffFreq(fco);
            filterBanks_[V3].setCutoffFreq(fco);
            break;
        }

        case RES_FILT:
            voicePassFilter_[V1] = data & (1 << 0);
            voicePassFilter_[V2] = data & (1 << 1);
            voicePassFilter_[V3] = data & (1 << 2);
            break;

        case MODE_VOL:
            volume_ = (float)(data & 0b00001111)/15.0;
            filterEnabled_[LP] = data & (1 << 4);
            filterEnabled_[BP] = data & (1 << 5);
            filterEnabled_[HP] = data & (1 << 6);
            break;

        default:
            if(voiceSetFcts_.count(addr))
            {
                voiceSetFcts_.at(addr)(data);
            }
            break;
        }
    }

    // Filter voices and mix
    float audioOut = 0.0;
    for(pair<const VoiceIdx, Voice>& voice : voices_)
    {
        voice.second.onSample(sampleCtr_);
        VoiceIdx v = voice.first;
        audioOut += filterBanks_[v].sample(
                voice.second.output(),
                voicePassFilter_[v] && filterEnabled_[LP],
                voicePassFilter_[v] && filterEnabled_[BP],
                voicePassFilter_[v] && filterEnabled_[HP]);
    }

    audioOut *= volume_ / (static_cast<int>(N_VOICES)*32767.0);

    return audioOut;
}


#if defined(REAL_SID_ATTACHED)

    SID::RealSID::RealSID()
    {
        nCommands_ = 0;
        fd_ = 0;
        int port = 0;
        ready_ = false;
        while(true)
        {
            stringstream ss;
            ss << "/dev/ttyACM" << port;
            string portname(ss.str());
            cout << "RealSID: Try to connect over " << portname << "...";
            fd_ = open(portname.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
            if(fd_ < 0)
            {
                cout << " no response." << endl;
                sleep_for(milliseconds(250));
                port++;
                if(port == 7)
                {
                    cout << "RealSID: Failed to establish a connection."
                         << endl;
                    port = 0;
                }
                continue;
            }
            else
            {
                cout << " connected to port " << port << "!\n";
                sleep_for(seconds(1));
                struct termios toptions;
                if(tcgetattr(fd_, &toptions) < 0)
                {
                    cout << "RealSID: Failed to get termios attributes."
                         << endl;
                    return;
                }
                cfsetispeed(&toptions, B500000);
                cfsetospeed(&toptions, B500000);
                toptions.c_cflag &= ~PARENB;
                toptions.c_cflag &= ~CSTOPB;
                toptions.c_cflag &= ~CSIZE;
                toptions.c_cflag |= CS8;
                toptions.c_cflag &= ~CRTSCTS;
                toptions.c_cflag |= CREAD | CLOCAL;
                toptions.c_iflag &= ~(IXON | IXOFF | IXANY);
                toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
                toptions.c_oflag &= ~OPOST;
                toptions.c_cc[VMIN]  = 0;
                toptions.c_cc[VTIME] = 50;
                if(tcsetattr(fd_, TCSANOW, &toptions) < 0)
                {
                    cout << "RealSID: Failed to set termios attributes."
                         << endl;
                    return;
                }
                cout << "RealSID: Wait for Arduino to get ready..." << endl;
                sleep_for(seconds(4));
                readEcho();
                cout << "RealSID: Connection OK.\n";
                break;
            }
        }
        ready_ = true;
    }

    bool SID::RealSID::connected() const
    {
        return ready_;
    }

    void SID::RealSID::readEcho()
    {
        cout << "Echo: " << flush;
        const int nTriesMax = 1000;
        int nTries = 0;
        while(true)
        {
            char c;
            int n = read(fd_, &c, 1);
            if((n == 0) || (n == -1 && errno == EAGAIN))
            {
                sleep_for(milliseconds(1));
                nTries++;
                if(nTries > nTriesMax)
                {
                    cout << "readEcho(): Warning: Nothing read from serial..."
                         << endl;
                    return;
                }
                else
                    continue;
            }
            else
            {
                nTries = 0;
                cout << c << flush;
                if(c==';')
                {
                    break;
                }
            }
        }
        return;
    }

    void SID::RealSID::send(SID::RealSID::Package a_pkg)
    {
        char msg[256] = { 0 };
        std::stringstream ss;
        ss << std::to_string((int)(a_pkg.addrbyte_)) << "/"
           << std::to_string((int)(a_pkg.databyte_)) << "/"
           << std::to_string((uint64_t)(a_pkg.sampleCtr_)) << ";";
        std::strcpy(msg, ss.str().c_str());
        int bytesSent = write(fd_, msg, strlen(msg));
        string ncs  = (a_pkg.addrbyte_&0b00100000) ? "HIGH" : "LOW";
        string nres = (a_pkg.addrbyte_&0b01000000) ? "HIGH" : "LOW";
        cout << "RealSID package: NCS " << ncs  << ", NRES " << nres
             << ", " << std::to_string((int)(a_pkg.addrbyte_ & 0b00011111))
             << "/" << to_string(a_pkg.databyte_) << "    (\"" << msg
             << "\"), bytesSent: " << bytesSent << endl;
        readEcho();
        nCommands_++;
        cout << endl;
        return;
    }

    void SID::RealSID::play()
    {
        cout << "RealSID::play(): " << nCommands_ << " commands had been sent."
             << endl;
        char msg[8] = { 0 };
        std::strcpy(msg, "p;");
        int bytesSent = write(fd_, msg, strlen(msg));
        cout << "RealSID package: \"" << msg << "\""
             << ", bytesSent: " << bytesSent << endl;
        readEcho();
        cout << endl;
    }

#endif // defined(REAL_SID_ATTACHED)

}  // namespace sid_types


