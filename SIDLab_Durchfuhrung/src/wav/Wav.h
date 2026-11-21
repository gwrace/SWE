/*! \file
 * \brief     .wav files stuff
 * \author    cpr
 * \date      2024
 * \copyright h_da fbeit
 */

#ifndef WAV_WAV_H_
#define WAV_WAV_H_

#include <fstream>
#include <vector>
#include <string>


//! Definitions dealing with the .wav file format
namespace wav_files
{

//! Saves and views a .wav-file
class Wav
{
public:
    //! Constructor
    /*!
     * \param a_audio The SID audio, as recorded by the SID_Driver,
     *        as 4-byte float values ranging from -1.0 to +1.0
     */
    Wav(const std::vector<float>& a_audio);

    //! Saves the waveform data in project folder "output" as a .wav-file
    /*!
     * \param a_filename The .wav filename, including extension
     * \return true if the file could be saved successfully, else false
     */
    bool saveAs(const std::string& a_filename) const;

    //! Views the waveform
    void view() const;

private:
    std::vector<float> audio_;  /*!< The SID audio, as recorded by the
    								 Driver, as 4-byte float values ranging
    								 from -1.0 to +1.0 */
};

}  // namespace wav_files


#endif /* WAV_WAV_H_ */
