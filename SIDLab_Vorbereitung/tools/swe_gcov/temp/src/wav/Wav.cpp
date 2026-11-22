/*! \file
 * \brief     .wav files stuff implementation
 * \author    cpr
 * \date      2024
 * \copyright h_da fbeit
 */

#include "Wav.h"
#include "tinywav.h"
#include "ghc/filesystem.hpp"

#include <iostream>
#include <algorithm>
#include <iterator>


namespace wav_files
{
    using namespace ghc::filesystem;
    using std::string;
    using std::vector;
    using std::cout;
    using std::endl;
    using std::size_t;
    using std::find_if;
    using std::distance;
    using std::rbegin;
    using std::rend;

    #if defined(__linux__)
        const std::string Wav::VIEW_CMD
            = "wine start ./tools/wavosaur/Wavosaur.exe ./output/temp.wav &";
    #elif defined (_WIN32)
        const std::string Wav::VIEW_CMD
            = "start ./tools/wavosaur/Wavosaur.exe ./output/temp.wav &";
    #else
        const std::string Wav::VIEW_CMD
            = "### UNSUPPORTED PLATFORM ###";
    #endif

    Wav::Wav(const vector<float>& a_audio) : audio_(a_audio)
    {
        saveAs("temp.wav");
    }

    bool Wav::saveAs(const string& a_filename) const
    {
        TinyWav tw;

        std::string path = std::string("./output/") + a_filename;
        cout << "Save waveform data as \"" << path << "\"." << std::endl;

        // Open output file
        if(tinywav_open_write(
            &tw,
            1,
            44100,
            TW_FLOAT32,
            TW_INLINE,
            path.c_str()) != 0)
        {
            return false;
        }

        // Crop silence at end
        const vector<float>& buf = audio_;
        auto result = find_if(rbegin(buf), rend(buf),
                              [](float f)
                              {
                                  return f!=0;
                              });
        size_t idx_last = distance(result, rend(buf));
        size_t size = idx_last+1;

        // Write
        for (size_t i = 0; i < size; i++)
        {
            const float* cfp = static_cast<const float*>(&(audio_[i]));
            const void* cvp = static_cast<const void*>(cfp);
            void* vp = const_cast<void*>(cvp);  // omg
            tinywav_write_f(&tw, vp, 1);
        }

        // Close file
        tinywav_close_write(&tw);
        return true;
    }

    void Wav::view() const
    {
        try
        {
            const auto copyOptions = copy_options::overwrite_existing;
            copy_file("./tools/wavosaur/wavosaur_old.cfg",
                      "./tools/wavosaur/wavosaur.cfg", copyOptions);
            copy_file("./tools/wavosaur/wavosaur_old.ini",
                      "./tools/wavosaur/wavosaur.ini", copyOptions);
        }
        catch(...)
        {
        }
        int ret = system(VIEW_CMD.c_str());
        cout << "system(\"" << VIEW_CMD << "\") returned with status "
             << ret << "." << endl;
        return;
    }

}  // namespace wav_files
