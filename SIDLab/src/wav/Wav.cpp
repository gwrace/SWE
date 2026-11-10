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
        vector<float> buf = audio_;
        auto result = find_if(rbegin(buf), rend(buf),
                              [](float f)
                              {
                                  return f!=0;
                              });
        size_t idx_last = distance(result, rend(buf))-1;
        size_t size = idx_last+1;

        // Write
        for (size_t i = 0; i < size; i++)
        {
            tinywav_write_f(&tw, static_cast<void*>(&(buf[i])), 1);
        }

        // Close file
        tinywav_close_write(&tw);
        return true;
    }

    void Wav::view() const
    {
		#if defined (_WIN32)
			string pth = current_path().string();
			string cmdLine_str = "explorer.exe \"" + pth + "\\tools\\waview\\view.bat\" &";
			int ret = system(cmdLine_str.c_str());
			cout << "system(\"\") call returned with status " << ret << "." << endl;

		#elif defined (__linux__)
			int ret = system("wine start ./tools/waview/waview_2.exe ./output/temp.wav &");
			cout << "system(\"\") call returned with status " << ret << "." << endl;
		#else
			cout << "Cannot view: Unsupported platform." << endl;
		#endif

        return;
    }

}  // namespace wav_files
