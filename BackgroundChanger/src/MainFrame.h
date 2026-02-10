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

/**
 * @brief The main window for this application.
 */
class MainFrame final : public wxFrame
{
public:
  /**
   * @brief The sole constructor for this class.
   *
   * @param[in] topLeftX Top left corner pixel x coordinate.
   * @param[in] topLeftY Top left corner pixel y coordinate.
   * @param[in] width Window width in pixels.
   * @param[in] height Window height in pixels.
   */
  explicit MainFrame(int topLeftX, int topLeftY, int width, int height);

protected:
  /**
   * @brief Intercepts the native windows message \c WM_SETTINGCHANGE.
   *
   * @param[in] message Message identifier.
   * @param[in] wParam Word-size additional message-dependent information.
   * @param[in] lParam Long-size additional message-dependent information.
   *
   * @returns The result of processing messages.
   */
  WXLRESULT MSWWindowProc (
    WXUINT message, 
    WXWPARAM wParam, 
    WXLPARAM lParam
  ) override;

private:
  /**
   * @brief Used when the \c Fill radio button is clicked.
   *
   * @param[in] event The specific event that activated this function.
   */
  void OnFillClicked (wxCommandEvent &event);

  /**
   * @brief Used when the \c Fit radio button is clicked.
   *
   * @param[in] event The specific event that activated this function.
   */
  void OnFitClicked (wxCommandEvent &event);

  /**
   * @brief Used when the \c Stretch radio button is clicked.
   *
   * @param[in] event The specific event that activated this function.
   */
  void OnStretchClicked (wxCommandEvent &event);

  /**
   * @brief Used when the \c Centre radio button is clicked.
   *
   * @param[in] event The specific event that activated this function.
   */
  void OnCentreClicked (wxCommandEvent &event);

  /**
   * @brief Used when the \c Tile radio button is clicked.
   *
   * @param[in] event The specific event that activated this function.
   */
  void OnTileClicked (wxCommandEvent &event);

  /**
   * @brief Used whenever an image thumbnail is selected.
   *
   * @param[in] event The specific event that activated this function.
   */
  void OnImageSelected (wxMouseEvent &event);

  /**
   * @brief Used when the main window is closed.
   *
   * @param[in] event The specific event that activated this function.
   */
  void OnClose (wxCloseEvent &event);

  /**
   * @brief Activated when a batch of data from the worker thread is ready.
   *
   * @param[in] event The specific event that activated this function.
   */
  void OnThumbnailsBatch (wxThreadEvent &event);

  /**
   * @brief Activated when the worker thread has processed the JSON manifest.
   *
   * @param[in] event The specific event that activated this function.
   */
  void OnThumbnailsStart (wxThreadEvent &event);

  /**
   * @brief Activated when there is no more data from the worker thread.
   *
   * @param[in] event The specific event that activated this function.
   */
  void OnThumbnailsDone (wxThreadEvent &event);

  /// The currently displayed image on the desktop background.
  std::wstring active_image_;

  /// The text shown in the status bar.
  wxStaticText *status_text_;

  /// The currently selected image to display onthe desktop background.
  wxPanel *selected_thumbnail_panel_;

  /// Sizer for the thumbnails.
  wxWrapSizer *wrap_sizer_;

  /// Sizer for scrolling the thumbnails.
  wxScrolledWindow *scrolled_window_;

  /// Progress bar for when the thumbnails are being loaded into their sizer.
  wxGauge *progress_bar_;

  /// Background worker thread for non-UI tasks.
  BackgroundWorkerThread *worker_;
};


//////////////////////////////////////////////////////////////////////////////
// END
//////////////////////////////////////////////////////////////////////////////
