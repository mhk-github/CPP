#pragma once
/**
 * @file MainFrame.h
 * @brief Header file for the MainFrame class.
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
// FORWARD DECLARATIONS
//////////////////////////////////////////////////////////////////////////////

class BackgroundWorkerThread;


//////////////////////////////////////////////////////////////////////////////
// CLASSES
//////////////////////////////////////////////////////////////////////////////

class MainFrame final : public wxFrame
{
public:
  explicit MainFrame(int, int, int, int);

protected:
  WXLRESULT MSWWindowProc (WXUINT, WXWPARAM, WXLPARAM) override;

private:
  void OnFillClicked (wxCommandEvent&);
  void OnFitClicked (wxCommandEvent&);
  void OnStretchClicked (wxCommandEvent&);
  void OnCentreClicked (wxCommandEvent&);
  void OnTileClicked (wxCommandEvent&);
  void OnImageSelected (wxMouseEvent&);
  void OnClose (wxCloseEvent&);
  void OnThumbnailsBatch(wxThreadEvent&);
  void OnThumbnailsStart(wxThreadEvent&);
  void OnThumbnailsDone(wxThreadEvent&);

  std::wstring active_image_;
  wxStaticText *status_text_;
  wxPanel *selected_thumbnail_panel_;
  wxWrapSizer *wrap_sizer_;
  wxScrolledWindow *scrolled_window_;
  wxGauge *progress_bar_;
  BackgroundWorkerThread *worker_;
};


//////////////////////////////////////////////////////////////////////////////
// END
//////////////////////////////////////////////////////////////////////////////
