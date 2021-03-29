#include <escher/nested_menu_controller.h>
#include <escher/container.h>
#include <assert.h>
#include <string.h>

namespace Escher {

/* Breadcrumb Controller */

NestedMenuController::BreadcrumbController::BreadcrumbController(Responder * parentResponder, SelectableTableView * tableView) :
  ViewController(parentResponder),
  m_selectableTableView(tableView),
  m_titleCount(0)
{
  updateTitle();
}

void NestedMenuController::BreadcrumbController::popTitle() {
  assert(m_titleCount > 0);
  m_titleCount--;
  m_titles[m_titleCount] = (I18n::Message)0;
  updateTitle();
}

void NestedMenuController::BreadcrumbController::pushTitle(I18n::Message title) {
  assert(m_titleCount < k_maxModelTreeDepth);
  m_titles[m_titleCount] = title;
  m_titleCount++;
  updateTitle();
}

void NestedMenuController::BreadcrumbController::resetTitle() {
  m_titleCount = 0;
  updateTitle();
}

void NestedMenuController::BreadcrumbController::updateTitle() {
  // m_titleCount == 0 is handled and only sets m_titleBuffer[0] to 0
  const char * separator = " > ";
  const int separatorLength = 3;
  // Define, from right to left, which subtitles will fit in the breadcrumb
  int titleLength = -separatorLength;
  int firstFittingSubtitleIndex = 0;
  for (int i = m_titleCount - 1; i >= 0; --i) {
    const int subtitleLength = strlen(I18n::translate(m_titles[i]));
    titleLength += separatorLength + subtitleLength;
    if (titleLength > k_maxTitleLength) {
      // This subtitle does not fit
      firstFittingSubtitleIndex = i + 1;
      break;
    }
  }
  // At least one subtitle should fit
  assert(m_titleCount == 0 || firstFittingSubtitleIndex < m_titleCount);
  // Build, from left to right, breadcrumb title from subtitles and separators
  int charIndex = 0;
  for (int i = firstFittingSubtitleIndex; i < m_titleCount; ++i) {
    // Separator ( only after first subtitle )
    if (i > firstFittingSubtitleIndex) {
      memcpy(m_titleBuffer + charIndex, separator, separatorLength);
      charIndex += separatorLength;
    }
    // Subtitle
    const char * subtitle = I18n::translate(m_titles[i]);
    const int subtitleLength = strlen(subtitle);
    memcpy(m_titleBuffer + charIndex, subtitle, subtitleLength);
    charIndex += subtitleLength;
  }
  assert(charIndex <= k_maxTitleLength);
  m_titleBuffer[charIndex] = 0;
}

/* State */

NestedMenuController::Stack::State::State(int selectedRow, KDCoordinate verticalScroll) :
  m_selectedRow(selectedRow),
  m_verticalScroll(verticalScroll)
{
}

bool NestedMenuController::Stack::State::isNull() const {
  if (m_selectedRow == -1) {
    return true;
  }
  return false;
}

/* Stack */
// Stack needs parent NestedMenuController to handle banners as it handles stack
NestedMenuController::Stack::Stack(NestedMenuController * parentMenu, SelectableTableView * tableView) :
  m_breadcrumbController(parentMenu, tableView),
  m_parentMenu(parentMenu)
{
}

void NestedMenuController::Stack::push(int selectedRow, KDCoordinate verticalScroll, I18n::Message title) {
  int stackDepth = depth();
  assert(m_statesStack[stackDepth].isNull());
  m_statesStack[stackDepth] = State(selectedRow, verticalScroll);
  /* Unless breadcrumb wasn't visible (depth 0), we need to pop it first to push
   * it again, in order to force title refresh. */
  if (stackDepth != 0) {
    m_parentMenu->pop();
  }
  m_breadcrumbController.pushTitle(title);
  m_parentMenu->push(&m_breadcrumbController);
}

NestedMenuController::Stack::State * NestedMenuController::Stack::stateAtIndex(int index) {
  return &m_statesStack[index];
}

NestedMenuController::Stack::State NestedMenuController::Stack::pop() {
  int stackDepth = depth();
  if (stackDepth == 0) {
    return State();
  }
  /* Unless breadcrumb is no longer visible (depth 1), we need to pop it first,
   * to push it again in order to force title refresh. */
  m_parentMenu->pop();
  m_breadcrumbController.popTitle();
  if (stackDepth != 1) {
    m_parentMenu->push(&m_breadcrumbController);
  }
  NestedMenuController::Stack::State state = m_statesStack[stackDepth-1];
  m_statesStack[stackDepth-1] = State();
  return state;
}

int NestedMenuController::Stack::depth() const {
  int depth = 0;
  for (int i = 0; i < k_maxModelTreeDepth; i++) {
    depth += (!m_statesStack[i].isNull());
  }
  return depth;
}

void NestedMenuController::Stack::resetStack() {
  for (int i = 0; i < k_maxModelTreeDepth; i++) {
    m_statesStack[i] = State();
  }
  m_breadcrumbController.resetTitle();
}

/* List Controller */

NestedMenuController::ListController::ListController(Responder * parentResponder, SelectableTableView * tableView, I18n::Message title) :
  ViewController(parentResponder),
  m_selectableTableView(tableView),
  m_firstSelectedRow(0),
  m_title(title)
{
}

const char * NestedMenuController::ListController::title() {
  return I18n::translate(m_title);
}

View * NestedMenuController::ListController::view() {
  return m_selectableTableView;
}

void NestedMenuController::ListController::didBecomeFirstResponder() {
  m_selectableTableView->reloadData();
  Container::activeApp()->setFirstResponder(m_selectableTableView);
  m_selectableTableView->selectCellAtLocation(0, m_firstSelectedRow);
}

/* NestedMenuController */

NestedMenuController::NestedMenuController(Responder * parentResponder, I18n::Message title) :
  StackViewController(parentResponder, &m_listController),
  m_selectableTableView(&m_listController, this, this, this),
  m_stack(this, &m_selectableTableView),
  m_listController(this, &m_selectableTableView, title),
  m_sender(nullptr)
{
  m_selectableTableView.setMargins(0);
  m_selectableTableView.setDecoratorType(ScrollView::Decorator::Type::None);
}

void NestedMenuController::setTitle(I18n::Message title) {
  m_listController.setTitle(title);
}

bool NestedMenuController::handleEvent(Ion::Events::Event event) {
  return handleEventForRow(event, selectedRow());
}

void NestedMenuController::didBecomeFirstResponder() {
  Container::activeApp()->setFirstResponder(&m_listController);
}

void NestedMenuController::viewWillAppear() {
  assert(depth() == 1 && stackDepth() == 0);
  // Reset memoization first, so that the right cells are manipulated
  resetMemoization();
  StackViewController::viewWillAppear();
  m_selectableTableView.reloadData();
  m_listController.setFirstSelectedRow(0);
}

void NestedMenuController::viewDidDisappear() {
  StackViewController::viewDidDisappear();
  m_stack.resetStack();
  m_selectableTableView.deselectTable();
}

HighlightCell * NestedMenuController::reusableCell(int index, int type) {
  assert(type < 2);
  assert(index >= 0);
  if (type == LeafCellType) {
    return leafCellAtIndex(index);
  }
  return nodeCellAtIndex(index);
}

int NestedMenuController::stackDepth() const {
  return m_stack.depth();
}

bool NestedMenuController::handleEventForRow(Ion::Events::Event event, int rowIndex) {
  int depth = m_stack.depth();
  if ((event == Ion::Events::Back || event == Ion::Events::Left) && depth > 0) {
    return returnToPreviousMenu();
  }
  if (selectedRow() < 0) {
    return false;
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE || event == Ion::Events::Right) && typeAtLocation(0, selectedRow()) == NodeCellType) {
    return selectSubMenu(rowIndex);
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && typeAtLocation(0, selectedRow()) == LeafCellType) {
    return selectLeaf(rowIndex);
  }
  return false;
}

bool NestedMenuController::selectSubMenu(int selectedRow) {
  resetMemoization();
  m_stack.push(selectedRow, m_selectableTableView.contentOffset().y(), subTitle());
  m_listController.setFirstSelectedRow(0);
  Container::activeApp()->setFirstResponder(&m_listController);
  return true;
}

bool NestedMenuController::returnToPreviousMenu() {
  assert(m_stack.depth() > 0);
  resetMemoization();
  NestedMenuController::Stack::State state = m_stack.pop();
  m_listController.setFirstSelectedRow(state.selectedRow() + stackRowOffset());
  KDPoint scroll = m_selectableTableView.contentOffset();
  m_selectableTableView.setContentOffset(KDPoint(scroll.x(), state.verticalScroll()));
  Container::activeApp()->setFirstResponder(&m_listController);
  return true;
}

}
