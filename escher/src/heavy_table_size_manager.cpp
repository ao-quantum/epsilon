#include <assert.h>
#include <escher/heavy_table_size_manager.h>

#include <algorithm>

namespace Escher {

KDCoordinate AbstractHeavyTableSizeManager::computeSizeAtIndex(
    int i, Dimension dimension) {
  KDCoordinate returnValue = memoizedSizeAtIndex(i, dimension);
  if (returnValue == TableSize1DManager::k_undefinedSize) {
    computeAllSizes();
    returnValue = memoizedSizeAtIndex(i, dimension);
    assert(returnValue != TableSize1DManager::k_undefinedSize);
  }
  return returnValue;
}

void AbstractHeavyTableSizeManager::resetAllSizes() {
  int nR = maxNumberOfRows();
  for (int i = 0; i < nR; i++) {
    *memoizedRowHeight(i) = TableSize1DManager::k_undefinedSize;
  }
  int nC = maxNumberOfColumns();
  for (int i = 0; i < nC; i++) {
    *memoizedColumnWidth(i) = TableSize1DManager::k_undefinedSize;
  }
}

void AbstractHeavyTableSizeManager::computeAllSizes() {
  int nR = maxNumberOfRows();
  int nC = maxNumberOfColumns();
  KDSize maxCellSize = m_delegate->maxCellSize();
  for (int c = 0; c < nC; c++) {
    for (int r = 0; r < nR; r++) {
      KDSize cellSize = m_delegate->cellSizeAtLocation(r, c);
      *memoizedRowHeight(r) = std::clamp(
          *memoizedRowHeight(r), cellSize.height(), maxCellSize.height());
      *memoizedColumnWidth(c) = std::clamp(
          *memoizedColumnWidth(c), cellSize.width(), maxCellSize.width());
    }
  }
}

}  // namespace Escher