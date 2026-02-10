/**
 * @file MainFrame.cpp
 * @brief Implementation file for the MainFrame class.
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
#include "Configuration.h"
#include "Events.h"
#include "Globals.h"
#include "MainFrame.h"
#include "Utilities.h"


///////////////////////////////////////////////////////////////////////////////
// USING
///////////////////////////////////////////////////////////////////////////////

// BOOST
using namespace boost::log::trivial;


//////////////////////////////////////////////////////////////////////////////
// CONSTANTS
//////////////////////////////////////////////////////////////////////////////

/**
 * @addtogroup CONSTANTS
 *
 * @{
 */

/// The title to set on the main window.
const auto * TITLE{L"BackgroundChanger (v1.1)"};

/// Status bar text during loading of image data.
const wxString LOADED_IMAGES{L"Loaded images"};

/// Status bar text for when the \c Fill radio button is clicked.
const wxString FILL_CHOSEN{L"'Fill' selected"};

/// Status bar text for when the \c Fit radio button is clicked.
const wxString FIT_CHOSEN{L"'Fit' selected"};

/// Status bar text for when the \c Stretch radio button is clicked.
const wxString STRETCH_CHOSEN{L"'Stretch' selected"};

/// Status bar text for when the \c Centre radio button is clicked.
const wxString CENTRE_CHOSEN{L"'Centre' selected"};

/// Status bar text for when the \c Tile radio button is clicked.
const wxString TILE_CHOSEN{L"'Tile' selected"};

/// The maximum width in pixels for a thumbnail image.
const auto THUMBNAIL_MAX_WIDTH{128};

/// The maximum height in pixels for a thumbnail image.
const auto THUMBNAIL_MAX_HEIGHT{128};

/**
 * @}
 */


//////////////////////////////////////////////////////////////////////////////
// CLASS IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////////

MainFrame::MainFrame(
  int topLeftX, 
  int topLeftY, 
  int width, 
  int height
)
  : wxFrame(
      nullptr,
      wxID_ANY,
      TITLE,
      wxPoint{topLeftX, topLeftY},
      wxSize{width, height}
    ),
    active_image_{},
    status_text_{nullptr},
    selected_thumbnail_panel_{nullptr},
    wrap_sizer_{nullptr},
    scrolled_window_{nullptr},
    progress_bar_{nullptr},
    worker_{nullptr}
{
  BOOST_LOG_SEV(g_Log,debug) << "  Images directory: '" 
    << wxTheApp->argv[1] << "'";
  BOOST_LOG_SEV(g_Log,debug) << "  Enter - MainFrame::MainFrame(" << topLeftX 
    << "," << topLeftY << "," << width << "," << height << ")";

  SetBackgroundColour (*wxLIGHT_GREY);

  // Root Layout
  auto * rootSizer{new wxBoxSizer{wxVERTICAL}};

  // Top Content Area
  auto * contentSizer{new wxBoxSizer{wxHORIZONTAL}};

  // Position Group
  auto * positionSizer{
    new wxStaticBoxSizer{wxVERTICAL, this, L"Position"}
  };
  positionSizer->SetMinSize (115, -1);

  auto * centre_rb{
    new wxRadioButton{
      this,
      wxID_ANY,
      L"Centre",
      wxDefaultPosition,
      wxDefaultSize,
      wxRB_GROUP
    }
  };
  auto * fill_rb{
    new wxRadioButton{
      this,
      wxID_ANY,
      L"Fill",
      wxDefaultPosition,
      wxDefaultSize,
      0
    }
  };
  auto * fit_rb{
    new wxRadioButton{
      this,
      wxID_ANY,
      L"Fit",
      wxDefaultPosition,
      wxDefaultSize,
      0
    }
  };
  auto * stretch_rb{
    new wxRadioButton{
      this,
      wxID_ANY,
      L"Stretch",
      wxDefaultPosition,
      wxDefaultSize,
      0
    }
  };
  auto * tile_rb{
    new wxRadioButton{
      this,
      wxID_ANY,
      L"Tile",
      wxDefaultPosition,
      wxDefaultSize,
      0
    }
  };

  fill_rb->SetToolTip (L"Fill the desktop background");
  fit_rb->SetToolTip (L"Fit to the desktop");
  stretch_rb->SetToolTip (L"Stretch the desktop background");
  centre_rb->SetToolTip (L"Centre the desktop background");
  tile_rb->SetToolTip (L"Tile the desktop background");

  fill_rb->Bind (wxEVT_RADIOBUTTON, &MainFrame::OnFillClicked, this);
  fit_rb->Bind (wxEVT_RADIOBUTTON, &MainFrame::OnFitClicked, this);
  stretch_rb->Bind (wxEVT_RADIOBUTTON, &MainFrame::OnStretchClicked, this);
  centre_rb->Bind (wxEVT_RADIOBUTTON, &MainFrame::OnCentreClicked, this);
  tile_rb->Bind (wxEVT_RADIOBUTTON, &MainFrame::OnTileClicked, this);

  positionSizer->Add (fill_rb, 0, wxALL | wxEXPAND, 2);
  positionSizer->Add (fit_rb, 0, wxALL | wxEXPAND, 2);
  positionSizer->Add (stretch_rb, 0, wxALL | wxEXPAND, 2);
  positionSizer->Add (centre_rb, 0, wxALL | wxEXPAND, 2);
  positionSizer->Add (tile_rb, 0, wxALL | wxEXPAND, 2);

  contentSizer->Add (positionSizer, 0, wxALL | wxEXPAND, 4);

  // Images Group
  auto * imagesSizer{new wxStaticBoxSizer{wxVERTICAL, this, L"Images"}};

  auto * scrolled{
    new wxScrolledWindow{
      this,
      wxID_ANY,
      wxDefaultPosition,
      wxDefaultSize,
      wxVSCROLL
    }
  };
  scrolled->Hide ();
  scrolled->SetBackgroundColour (*wxBLACK);

  auto * wrap_sizer{new wxWrapSizer{wxHORIZONTAL}};

  scrolled->Bind (
    wxEVT_SIZE, 
    [wrap_sizer, scrolled] (wxSizeEvent& event)
    {
      wrap_sizer->SetDimension (0, 0, scrolled->GetClientSize ().x, -1);
      wrap_sizer->Layout ();
      scrolled->FitInside ();
      event.Skip ();
    }
  );
  scrolled->SetSizer (wrap_sizer);

  imagesSizer->Add (scrolled, 1, wxEXPAND | wxALL, 2);
  contentSizer->Add (imagesSizer, 1, wxALL | wxEXPAND, 4);
  rootSizer->Add (contentSizer, 1, wxEXPAND);

  // Bottom Status Area
  auto * bottomSizer{new wxBoxSizer(wxHORIZONTAL)};

  auto * statusPanel{new wxPanel{this}};
  statusPanel->SetMinSize (wxSize{-1, 32});
  statusPanel->SetBackgroundColour (*wxLIGHT_GREY);

  auto * statusSizer{new wxBoxSizer{wxHORIZONTAL}};

  auto * status_text{
    new wxStaticText{
      statusPanel,
      wxID_ANY,
      wxEmptyString
    }
  };

  auto * progress_bar{
    new wxGauge{
      statusPanel,
      wxID_ANY,
      100,
      wxDefaultPosition,
      wxDefaultSize,
      wxGA_HORIZONTAL
    }
  };
  progress_bar->SetMinSize (wxSize{-1, 26});
  progress_bar->Hide ();

  statusSizer->Add (
    status_text,
    1,
    wxALIGN_CENTER_VERTICAL | wxLEFT,
    5
  );
  statusSizer->Add (
    progress_bar, 
    0, 
    wxALIGN_CENTER_VERTICAL | wxRIGHT,
    5
  );

  statusPanel->SetSizer (statusSizer);

  bottomSizer->Add (statusPanel, 1, wxEXPAND);

  rootSizer->Add (bottomSizer, 0, wxEXPAND);

  SetSizer (rootSizer);
  Layout ();

  Bind (wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);
  Bind (wxEVT_SYS_COLOUR_CHANGED, &MainFrame::OnSysColourChanged, this);
  Bind (EVT_THUMBNAILS_BATCH, &MainFrame::OnThumbnailsBatch, this);
  Bind (EVT_THUMBNAILS_START, &MainFrame::OnThumbnailsStart, this);
  Bind (EVT_THUMBNAILS_DONE, &MainFrame::OnThumbnailsDone, this);

  const auto config{GetConfiguration ()};
  if (config == nullptr)
  {
    wxLogError (INI_FILE_ERROR);
    BOOST_LOG_SEV(g_Log,fatal) << "    " << INI_FILE_ERROR;
  }
  assert(config != nullptr);

  wxString jsonFile{};
  config->Read (MANIFEST_KEY, &jsonFile, DEFAULT_MANIFEST);
  int batchSize{};
  config->Read (BATCH_SIZE_KEY, &batchSize, DEFAULT_BATCH_SIZE);
  int sleepTime{};
  config->Read (SLEEP_TIME_KEY, &sleepTime, DEFAULT_SLEEP_TIME);

  status_text_= status_text;
  wrap_sizer_ = wrap_sizer;
  scrolled_window_ = scrolled;
  progress_bar_= progress_bar;
  worker_ = new BackgroundWorkerThread{
    this,
    jsonFile.ToStdString (),
    wxTheApp->argv[1].ToStdString (),
    static_cast<std::size_t>(batchSize),
    static_cast<unsigned long>(sleepTime)
  };
  worker_->Run ();

  BOOST_LOG_SEV(g_Log,debug) << "  Leave - MainFrame::MainFrame()";
}

void MainFrame::OnFillClicked (wxCommandEvent &event)
{
  BOOST_LOG_SEV(g_Log,debug) << "  Enter MainFrame::OnFillClicked(...)";
  UpdateRegistry (WALLPAPER_FILL, false);
  ChangeDesktopBackground (active_image_);
  status_text_->SetLabel (FILL_CHOSEN);
  BOOST_LOG_SEV(g_Log,debug) << "  Leave MainFrame::OnFillClicked(...)";
}

void MainFrame::OnFitClicked (wxCommandEvent &event)
{
  BOOST_LOG_SEV(g_Log,debug) << "  Enter MainFrame::OnFitClicked(...)";
  UpdateRegistry (WALLPAPER_FIT, false);
  ChangeDesktopBackground (active_image_);
  status_text_->SetLabel (FIT_CHOSEN);
  BOOST_LOG_SEV(g_Log,debug) << "  Leave MainFrame::OnFitClicked(...)";
}

void MainFrame::OnStretchClicked (wxCommandEvent &event)
{
  BOOST_LOG_SEV(g_Log,debug) << "  Enter MainFrame::OnStretchClicked(...)";
  UpdateRegistry (WALLPAPER_STRETCH, false);
  ChangeDesktopBackground (active_image_);
  status_text_->SetLabel (STRETCH_CHOSEN);
  BOOST_LOG_SEV(g_Log,debug) << "  Leave MainFrame::OnStretchClicked(...)";
}

void MainFrame::OnCentreClicked (wxCommandEvent &event)
{
  BOOST_LOG_SEV(g_Log,debug) << "  Enter MainFrame::OnCentreClicked(...)";
  UpdateRegistry (WALLPAPER_CENTRE, false);
  ChangeDesktopBackground (active_image_);
  status_text_->SetLabel (CENTRE_CHOSEN);
  BOOST_LOG_SEV(g_Log,debug) << "  Leave MainFrame::OnCentreClicked(...)";
}

void MainFrame::OnTileClicked (wxCommandEvent &event)
{
  BOOST_LOG_SEV(g_Log,debug) << "  Enter MainFrame::OnTileClicked(...)";
  UpdateRegistry (WALLPAPER_CENTRE, true);
  ChangeDesktopBackground (active_image_);
  status_text_->SetLabel (TILE_CHOSEN);
  BOOST_LOG_SEV(g_Log,debug) << "  Leave MainFrame::OnTileClicked(...)";
}

void MainFrame::OnImageSelected (wxMouseEvent &event)
{
  BOOST_LOG_SEV(g_Log,debug) << "  Enter MainFrame::OnImageSelected(...)";

  auto * imgCtrl = static_cast<wxWindow*>(event.GetEventObject ());
  auto * thumbPanel = static_cast<wxPanel*>(imgCtrl->GetParent ());

  if (selected_thumbnail_panel_)
  {
    selected_thumbnail_panel_->SetBackgroundColour (*wxBLACK);
    selected_thumbnail_panel_->Refresh ();
  }
  selected_thumbnail_panel_ = thumbPanel;
  selected_thumbnail_panel_->SetBackgroundColour (*wxBLUE);
  selected_thumbnail_panel_->Refresh ();

  const auto value{imgCtrl->GetName ()};
  active_image_ = value.ToStdWstring ();
  ChangeDesktopBackground (active_image_);
  status_text_->SetLabel (
    wxString::Format (
      "Selected '%s'", 
      value
    )
  );

  BOOST_LOG_SEV(g_Log,debug) << "    Selected '" << value << "'";
  BOOST_LOG_SEV(g_Log,debug) << "  Leave MainFrame::OnImageSelected(...)";
}

void MainFrame::OnClose (wxCloseEvent &event)
{
  BOOST_LOG_SEV(g_Log,debug) << "  Enter MainFrame::OnClose(...)";

  if (worker_)
  {
    if (worker_->IsRunning ())
    {
      worker_->Delete (); // Request cancellation
      worker_->Wait ();   // Block until thread exits
    }
    delete worker_;
    worker_ = nullptr;
  }

  BOOST_LOG_SEV(g_Log,debug) << "  Leave MainFrame::OnClose(...)";
  Destroy ();
}

void MainFrame::OnThumbnailsBatch (wxThreadEvent &event)
{
  BOOST_LOG_SEV(g_Log,debug) << "  Enter - MainFrame::OnThumbnailsBatch(...)";

  wxLogNull log_null{};

  const auto batch{event.GetPayload<std::vector<JSONData>> ()};
  for (const auto & item : batch)
  {
    wxBitmap bitmap{item.cache_file_path, wxBITMAP_TYPE_BMP};
    if (!bitmap.IsOk ())
    {
      BOOST_LOG_SEV(g_Log,warning) << "    Error loading '" 
        << item.cache_file_path << "' !";
      continue;
    }

    auto * thumbPanel{
      new wxPanel{
        scrolled_window_,
        wxID_ANY,
        wxDefaultPosition,
        wxSize{THUMBNAIL_MAX_WIDTH, THUMBNAIL_MAX_HEIGHT}
      }
    };
    thumbPanel->SetBackgroundColour (*wxBLACK);
    auto * sizer{new wxBoxSizer{wxVERTICAL}};
    auto * img = new wxStaticBitmap{
      thumbPanel,
      wxID_ANY,
      bitmap
    };
    img->SetName (item.file_path);
    img->SetToolTip (
      wxString::Format (
        L"File: %s\nWidth: %d\nHeight: %d",
        wxFileName{item.file_path}.GetFullName (),
        item.width,
        item.height
      )
    );
    img->Bind (wxEVT_LEFT_DOWN, &MainFrame::OnImageSelected, this);
    sizer->Add (img, 1, wxALIGN_CENTER);
    thumbPanel->SetSizer (sizer);

    wrap_sizer_->Add (thumbPanel, 0, wxALL, 2);
  }

  progress_bar_->SetValue (progress_bar_->GetValue () + batch.size ());

  BOOST_LOG_SEV(g_Log,debug) << "  Leave - MainFrame::OnThumbnailsBatch(...)";
}

void MainFrame::OnThumbnailsStart (wxThreadEvent &event)
{
  BOOST_LOG_SEV(g_Log,debug) << "  Enter - MainFrame::OnThumbnailsStart(...)";

  const auto total_images{event.GetPayload<std::size_t> ()};
  BOOST_LOG_SEV(g_Log,debug) << "    Images count = " << total_images;

  progress_bar_->SetRange (total_images);
  progress_bar_->SetValue (0);
  progress_bar_->Show ();
  Layout ();

  BOOST_LOG_SEV(g_Log,debug) << "  Leave - MainFrame::OnThumbnailsStart(...)";
}

void MainFrame::OnThumbnailsDone (wxThreadEvent &event)
{
  BOOST_LOG_SEV(g_Log,debug) << "  Enter - MainFrame::OnThumbnailsDone(...)";

  scrolled_window_->GetParent ()->Layout ();
  scrolled_window_->SetScrollRate (0, 10);
  scrolled_window_->Show ();

  progress_bar_->Hide ();

  status_text_->SetLabel (LOADED_IMAGES);

  Layout ();

  BOOST_LOG_SEV(g_Log,debug) << "  Leave - MainFrame::OnThumbnailsDone(...)";
}

WXLRESULT MainFrame::MSWWindowProc (
  WXUINT message,
  WXWPARAM wParam,
  WXLPARAM lParam
)
{
  if (message == WM_SETTINGCHANGE)
  {
    BOOST_LOG_SEV(g_Log,debug)
      << "  WM_SETTINGCHANGE detected [wParam=" << wParam << "]";

    ChangeDesktopBackground (active_image_);
  }

  return wxFrame::MSWWindowProc (message, wParam, lParam);
}


//////////////////////////////////////////////////////////////////////////////
// END
//////////////////////////////////////////////////////////////////////////////
