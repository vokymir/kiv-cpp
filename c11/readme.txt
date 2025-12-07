# fixes

- add missing includes
- remove unused includes
- IAnalyzer missing destructor *virtual* & Dist2DAnalyzer missing *override* on virtual destructor
- 2DAnalyzer::Load()
  - read once
- 2DAnalyzer::Analyze()
  - remove try/catch
  - preallocate Histogram before loop
  - only one loop for creating Histogram & finding mMaximum
- PPMDrawing::Save() ofs.write() does write the whole row
  - IDrawing::Color have new fuctions to allow for the above
