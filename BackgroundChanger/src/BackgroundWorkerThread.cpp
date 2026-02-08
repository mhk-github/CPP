/**
 * @file BackgroundWorkerThread.cpp
 * @brief Implementation file for the BackgroundWorkerThread class.
 *
 * @date @showdate "%d %B %Y"
 * @copyright MIT License.
 */


//////////////////////////////////////////////////////////////////////////////
// HEADER FILES
//////////////////////////////////////////////////////////////////////////////

// PRECOMPILED HEADER FILE ///////////////////////////////////////////////////
#include "PCH.h"

#include "BackgroundWorkerThread.h"
#include "Events.h"
#include "Globals.h"
#include "Utilities.h"


///////////////////////////////////////////////////////////////////////////////
// USING
///////////////////////////////////////////////////////////////////////////////

// BOOST
using namespace boost::log::trivial;


//////////////////////////////////////////////////////////////////////////////
// CLASS IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////

BackgroundWorkerThread::BackgroundWorkerThread(
  wxEvtHandler *handler,
  std::string jsonFile,
  std::string imagesDir,
  std::size_t batchSize,
  unsigned long sleepTime
)
  : wxThread{wxTHREAD_JOINABLE},
  handler_{handler},
  json_file_{std::move (jsonFile)},
  images_dir_{std::move (imagesDir)},
  batch_size_{batchSize},
  sleep_time_{sleepTime}
{
  BOOST_LOG_SEV(g_Log,debug) << 
    "    Enter - BackgroundWorkerThread::BackgroundWorkerThread(" << handler 
    << ",'" << json_file_ << "','" << images_dir_ << "'," << batch_size_ 
    << "," << sleep_time_ << ")";
  BOOST_LOG_SEV(g_Log,debug) << 
    "    Leave - BackgroundWorkerThread::BackgroundWorkerThread(...)";
}

wxThread::ExitCode BackgroundWorkerThread::Entry ()
{
  BOOST_LOG_SEV(g_Log,debug) << "  Enter - BackgroundWorkerThread::Entry ()";

  const auto items{ProcessJSONFile (json_file_, images_dir_)};
  std::vector<JSONData> batch{};
  batch.reserve (batch_size_);

  auto start_evt{new wxThreadEvent{EVT_THUMBNAILS_START}};
  start_evt->SetPayload<std::size_t> (items.size ());
  wxQueueEvent (handler_, start_evt);

  // Allow time for the UI to be set up
  wxMilliSleep (sleep_time_);

  for (const auto &item : items)
  {
    if (TestDestroy ())
    {
      break;
    }

    batch.push_back (
      {
        item.file_path,
        item.cache_file_path,
        item.width,
        item.height
      }
    );

    if (batch.size () == batch_size_)
    {
      auto* batch_evt{new wxThreadEvent{EVT_THUMBNAILS_BATCH}};
      batch_evt->SetPayload (batch);
      wxQueueEvent (handler_, batch_evt);
      batch.clear ();
    }
  }

  if (!batch.empty ())
  {
    auto* last_batch_evt{new wxThreadEvent{EVT_THUMBNAILS_BATCH}};
    last_batch_evt->SetPayload (batch);
    wxQueueEvent (handler_, last_batch_evt);
  }
  wxQueueEvent (handler_, new wxThreadEvent{EVT_THUMBNAILS_DONE});

  BOOST_LOG_SEV(g_Log,debug) << "  Leave - BackgroundWorkerThread::Entry ()";
  return (wxThread::ExitCode)0;
}


//////////////////////////////////////////////////////////////////////////////
// END
//////////////////////////////////////////////////////////////////////////////
