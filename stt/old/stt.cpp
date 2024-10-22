/* Project: occ-accessibility-tour
 * Sub-project: voice
 * Developers: @agge3, @kpowkitty
 * Date: February 2024
 */

#include "deepspeech.h"

#include <stdlib.h>
#include <stdio.h>

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <sstream>
#include <string>
#include <vector>
#include <iostream>

char* model = NULL;

char* scorer = NULL;

char* audio = NULL;

bool set_beamwidth = false;

int beam_width = 0;

bool set_alphabeta = false;

float lm_alpha = 0.f;

float lm_beta = 0.f;

bool show_times = false;

bool has_versions = false;

bool extended_metadata = false;

bool json_output = false;

int json_candidate_transcripts = 3;

int stream_size = 0;

int extended_stream_size = 0;

char* hot_words = NULL;

typedef struct {
    char* buffer;
    size_t buffer_size;
} ds_audio_buffer;

/**
 * @note 
 * struct CandidateTranscript {
 *  const TokenMetadata* const tokens
 *  const unsigned_int num_tokens
 *  const double confidence
 *  };
 * @note
 * struct TokenMetadata {
 *  const char* const text
 *  const unsigned_int timestep
 *  const float start_time
 * };
 * @note
 * struct Metadata {
 *  const CandidateTranscript* const transcripts
 *  const unsigned_int num_transcripts
 * };
 */

// TODO: Do we need?width
// char* CandidateTranscriptToString(const CandidateTranscript* transcript)
// {
//   std::string retval = "";
//   for (int i = 0; i < transcript->num_tokens; i++) {
//     const TokenMetadata& token = transcript->tokens[i];
//     retval += token.text;
//   }
//   return strdup(retval.c_str());
// }

/**
* Get audio buffer.
* @return ds_audio_buffer
* @todo Do we need?
*/
// ds_audio_buffer GetAudioBuffer(const char* path, int desired_sample_rate)
// {
//   ds_audio_buffer res = {0};
// 
//   assert(output);
// 
//     // NOTE: Needs specific sample rate ... LOOK INTO
// 
// 
//   if ((int)input->signal.rate < desired_sample_rate) {
//     fprintf(stderr, "Warning: original sample rate (%d) is lower than %dkHz. "
//                     "Up-sampling might produce erratic speech recognition.\n",
//                     desired_sample_rate, (int)input->signal.rate);
//   }
// 
// 
// 
// 
//   return res;
// }

// TODO: Do we need?
// void ProcessFile(ModelState* context, const char* path, bool show_times)
// {
//   ds_audio_buffer audio = GetAudioBuffer(path, DS_GetModelSampleRate(context));
// 
//   // Pass audio to DeepSpeech
//   // We take half of buffer_size because buffer is a char* while
//   // LocalDsSTT() expected a short*
//   ds_result result = LocalDsSTT(context,
//                                 (const short*)audio.buffer,
//                                 audio.buffer_size / 2,
//                                 extended_metadata,
//                                 json_output);
//   free(audio.buffer);
// 
//   if (result.string) {
//     printf("%s\n", result.string);
//     DS_FreeString((char*)result.string);
//   }
// 
//   if (show_times) {
//     printf("cpu_time_overall=%.05f\n",
//            result.cpu_time_overall);
//   }
// }



int main() {
    // init ds 
    ModelState* ctx;

    // model & scorer path
    static const char mpath[] = "../dep/deepspeech-models/deepspeech-0.9.3-models.pbmm";
    static const char spath[] = "../dep/deepspeech-models/deepspeech-0.9.3-models.scorer";
    
    // sphinx-doc: c_ref_model_start
    
    int status = DS_CreateModel(mpath, &ctx);
    if (status != 0) {
        char* error = DS_ErrorCodeToErrorMessage(status);
        fprintf(stderr, "Could not create model: %s\n", error);
        free(error);
        return 1;
    }

    // model's created
    
    // TODO: beamwidth? 

    // now scorer

    status = DS_EnableExternalScorer(ctx, spath);
    if (status != 0) {
        fprintf(stderr, "Could not enable external scorer.\n");
        return 1;
    // TODO: alpha/beta?
    }

    // util: get expected model sample rate
    //if (DS_GetModelSampleRate(ctx) != sample rate we have...) {
    
    int sample_rate = DS_GetModelSampleRate(ctx);
    std::cout << sample_rate << "\n";


    // NOTE: 16-bit, mono raw audio signal, SAMPLE RATE IS 16000
    // NOTE: sf::SoundStream?
    // NOTE: DS_FeedAudioContent(
    
    StreamingState* sctx; // opaque ptr that returns NULL on err

    // STT

    
    // ctx (local), short int 
    //DS_SpeechToText(ctx, abuf, asize);
    // need to DS_FreeString from mem
    // is going to ret char*



    /************************** TODO *******************************/
    // TODO: hot words?
    // sphinx-doc: c_ref_model_stop
    // if (hot_words) {
    //   std::vector<std::string> hot_words_ = SplitStringOnDelim(hot_words, ",");
    //   for ( std::string hot_word_ : hot_words_ ) {
    //     std::vector<std::string> pair_ = SplitStringOnDelim(hot_word_, ":");
    //     const char* word = (pair_[0]).c_str();
    //     // the strtof function will return 0 in case of non numeric characters
    //     // so, check the boost string before we turn it into a float
    //     bool boost_is_valid = (pair_[1].find_first_not_of("-.0123456789") == std::string::npos);
    //     float boost = strtof((pair_[1]).c_str(),0);
    //     status = DS_AddHotWord(ctx, word, boost);
    //     if (status != 0 || !boost_is_valid) {
    //       fprintf(stderr, "Could not enable hot-word.\n");
    //       return 1;
    //     }
    //   }
    // }
  
    // TODO: Maybe not necc?
    // struct stat wav_info;
    // if (0 != stat(audio, &wav_info)) {
    //   printf("Error on stat: %d\n", errno);
    // }
  
    // switch (wav_info.st_mode & S_IFMT) {
    //   case S_IFREG:
    //       ProcessFile(ctx, audio, show_times);
    //     break;
  
    //   case S_IFDIR:
    //       {
    //         printf("Running on directory %s\n", audio);
    //         DIR* wav_dir = opendir(audio);
    //         assert(wav_dir);
  
    //         struct dirent* entry;
    //         while ((entry = readdir(wav_dir)) != NULL) {
    //           std::string fname = std::string(entry->d_name);
    //           if (fname.find(".wav") == std::string::npos) {
    //             continue;
    //           }
  
    //           std::ostringstream fullpath;
    //           fullpath << audio << "/" << fname;
    //           std::string path = fullpath.str();
    //           printf("> %s\n", path.c_str());
    //           ProcessFile(ctx, path.c_str(), show_times);
    //         }
    //         closedir(wav_dir);
    //       }
    //     break;
  
    //   default:
    //       printf("Unexpected type for %s: %d\n", audio, (wav_info.st_mode & S_IFMT));
    //     break;
    // }
    /************************* END TODO ****************************/
  
    // grab and parse txt

    DS_FreeModel(ctx);
    return 0;
}
