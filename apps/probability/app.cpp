#include "app.h"

#include <apps/shared/text_field_delegate_app.h>
#include <apps/exam_mode_configuration.h>

#include "models/statistic/homogeneity_test.h"
#include "probability_icon.h"

namespace Probability {

const Escher::Image * App::Descriptor::icon() const {
  return ImageStore::ProbabilityIcon;
}

App::App(Snapshot * snapshot) :
    TextFieldDelegateApp(snapshot, &m_stackViewController),
    m_testGraphController(&m_stackViewController, static_cast<Test *>(snapshot->statistic())),
    m_intervalGraphController(&m_stackViewController, static_cast<Interval *>(snapshot->statistic())),
    m_homogeneityResultsController(
        &m_stackViewController,
        static_cast<HomogeneityTest *>(snapshot->statistic()),
        &m_resultsController),
    m_inputHomogeneityController(
        &m_stackViewController,
        &m_homogeneityResultsController,
        this,
        static_cast<HomogeneityTest *>(snapshot->statistic())),
    m_inputGoodnessController(&m_stackViewController,
                              &m_resultsController,
                              static_cast<GoodnessTest *>(snapshot->statistic()),
                              this),
    m_resultsController(&m_stackViewController,
                        snapshot->statistic(),
                        &m_testGraphController,
                        &m_intervalGraphController,
                        this,
                        this),
    m_inputController(&m_stackViewController,
                      &m_resultsController,
                      snapshot->statistic(),
                      this),
    m_typeController(&m_stackViewController,
                     &m_hypothesisController,
                     &m_inputController,
                     snapshot->statistic()),
    m_categoricalTypeController(&m_stackViewController,
                                static_cast<Chi2Test *>(snapshot->statistic()),
                                &m_inputGoodnessController,
                                &m_inputHomogeneityController),
    m_hypothesisController(&m_stackViewController,
                           &m_inputController,
                           this,
                           static_cast<Test *>(snapshot->statistic())),
    m_calculationController(&m_stackViewController,
                            this,
                            snapshot->distribution(),
                            snapshot->calculation()),
    m_parameterController(&m_stackViewController,
                          this,
                          snapshot->distribution(),
                          &m_calculationController),
    m_distributionController(&m_stackViewController,
                             snapshot->distribution(),
                             &m_parameterController),
    m_testController(&m_stackViewController,
                     &m_hypothesisController,
                     &m_typeController,
                     &m_categoricalTypeController,
                     &m_inputController,
                     snapshot->statistic()),
    m_menuController(&m_stackViewController,
                     &m_distributionController,
                     &m_testController,
                     snapshot->inference()),
    m_stackViewController(&m_modalViewController, &m_menuController, StackViewController::Style::GrayGradation),
    m_bufferDestructor(nullptr)
{
}

void App::didBecomeActive(Window * windows) {
  Ion::RingBuffer<Escher::ViewController *, Escher::k_MaxNumberOfStacks> * queue = snapshot()->pageQueue();
  int queueLength = queue->length();
  Escher::ViewController * currentController = &m_menuController;
  for (int i = 0; i < queueLength; i++) {
    /* The queue is refilled dynamically when "stackOpenPage"ing which prevents
     * from popping until the queue is empty. */
    Escher::ViewController * controller = queue->queuePop();
    currentController->stackOpenPage(controller);
  }
  Escher::App::didBecomeActive(windows);
}

#if 0
void App::initTableSelections(Data::Page page,
                              Data::SubApp subapp,
                              Data::Test test,
                              Data::TestType type,
                              Data::CategoricalType categoricalType) {
  /* Set correct selection for list controllers
  when recreating App */
  if (page != Data::Page::Menu) {
    // Menu selection
    if (subapp == Data::SubApp::Probability) {
      m_menuController.selectRow(MenuController::k_indexOfProbability);
    } else if (subapp == Data::SubApp::Tests) {
      m_menuController.selectRow(MenuController::k_indexOfTest);
    } else if (subapp == Data::SubApp::Intervals) {
      m_menuController.selectRow(MenuController::k_indexOfInterval);
    }

    if (subapp != Data::SubApp::Probability && page != Data::Page::Test) {
      // Test selection
      switch (test) {
        case Data::Test::OneProp:
          m_testController.selectRow(TestController::k_indexOfOneProp);
          break;
        case Data::Test::OneMean:
          m_testController.selectRow(TestController::k_indexOfOneMean);
          break;
        case Data::Test::TwoProps:
          m_testController.selectRow(TestController::k_indexOfTwoProps);
          break;
        case Data::Test::TwoMeans:
          m_testController.selectRow(TestController::k_indexOfTwoMeans);
          break;
        case Data::Test::Categorical:
          m_testController.selectRow(TestController::k_indexOfCategorical);
          break;
        default:
          assert(false);
          break;
      }

      int row;
      if (page != Data::Page::Type &&
          (test == Data::Test::OneMean || test == Data::Test::TwoMeans)) {
        // Type selection
        switch (type) {
          case Data::TestType::TTest:
            m_typeController.selectRow(TypeController::k_indexOfTTest);
            break;
          case Data::TestType::ZTest:
            row = TypeController::k_indexOfZTest;
            if (test == Data::Test::OneMean) {
              row--;  // PooledTTest row is not shown
            }
            m_typeController.selectRow(row);
            break;
          case Data::TestType::PooledTTest:
            m_typeController.selectRow(TypeController::k_indexOfPooledTest);
            break;
          default:
            assert(false);
            break;
        }
      } else if (test == Data::Test::Categorical && page != Data::Page::Categorical) {
        m_categoricalTypeController.selectRow(
            categoricalType == Data::CategoricalType::Goodness
                ? CategoricalTypeController::k_indexOfGoodnessCell
                : CategoricalTypeController::k_indexOfHomogeneityCell);
      }
    }
  }
}
#endif

void App::willOpenPage(ViewController * controller) {
  snapshot()->pageQueue()->push(controller);
}

void App::didExitPage(ViewController * controller) {
  ViewController * c = snapshot()->pageQueue()->stackPop();
  assert(c == controller);
}

void App::cleanBuffer(DynamicCellsDataSourceDestructor * destructor) {
  assert(destructor);
  if (m_bufferDestructor) {
    m_bufferDestructor->destroyCells();
  }
  m_bufferDestructor = destructor;
}

const App::Descriptor * App::Snapshot::descriptor() const {
  static App::Descriptor s_descriptor;
  return &s_descriptor;
}

void App::Snapshot::tidy() {
  inference()->tidy();
}

void App::Snapshot::reset() {
  m_pageQueue.reset();
}

}  // namespace Probability
