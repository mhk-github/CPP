#pragma once
/**
 * @file BackgroundWorkerThread.h
 * @brief Header file for the BackgroundWorkerThread class.
 *
 * @date @showdate "%d %B %Y"
 * @copyright MIT License.
 */


//////////////////////////////////////////////////////////////////////////////
// HEADER FILES
//////////////////////////////////////////////////////////////////////////////

// PRECOMPILED HEADER FILE ///////////////////////////////////////////////////
#include "PCH.h"


//////////////////////////////////////////////////////////////////////////////
// CLASS DECLARATIONS
//////////////////////////////////////////////////////////////////////////////

/**
 * @brief Background worker thread class for non-UI tasks.
 *
 * @ingroup THREADS
 */
class BackgroundWorkerThread final : public wxThread
{
public:
  /**
   * @brief The sole constructor for this class.
   *
   * @param[in] handler Event processing manager.
   * @param[in] jsonFile Canonical path to a JSON manifest file.
   * @param[in] imagesDir Canonical path to a directory with image files.
   * @param[in] batchSize Maximum number of data items to give in batches.
   * @param[in] sleepTime Sleep time in milliseconds so the UI can set up.
   *
   * @internal @warning Do not use std::wstring !
   */
  explicit BackgroundWorkerThread(
    wxEvtHandler *handler, 
    std::string jsonFile, 
    std::string imagesDir,
    std::size_t batchSize,
    unsigned long sleepTime
  );

protected:
  /**
   * @brief The thread entry point.
   *
   * @returns Specific exit code depending on task success or not.
   */
  ExitCode Entry () override;

private:
  /// The object responsible for event processing.
  wxEvtHandler *handler_;

  /// Canonical path a JSON manifest file for images data.
  std::string json_file_;

  /// Canonical path to a directory with image files.
  std::string images_dir_;

  /// Maximum number of data items to return in each batch.
  std::size_t batch_size_;

  /// Number of milliseconds to sleep to allow the UI to set up.
  unsigned long sleep_time_;
};


//////////////////////////////////////////////////////////////////////////////
// END
//////////////////////////////////////////////////////////////////////////////
