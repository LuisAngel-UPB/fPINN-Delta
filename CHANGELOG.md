# Changelog

## v1.3.0 (2026-11)
- Added `NB09_fPINN_TiempoC.ipynb`, `c/NB09_fPINN_timing_bench.c`, `c/NB07_fPINN_memkernels_lib.h` and `results/NB09_fPINN_TimingC_Repetido.csv`: repeated timing of the fixed-cost C kernels with precomputed inputs, loop overhead and per-update percentiles.
- Note: the NB06/NB07 files listed under v1.1.0 were first committed with v1.2.0 (the v1.1.0 tag archive lacks them); this release contains everything.

## v1.2.0 (2026-10-14)
- Added `NB08_fPINN_PlantaMulticuerpo.ipynb`: constrained multibody plant of the delta robot (benchmark B), with a Caputo damper or LuGre friction in the joints, PINN and fPINN with seeds 0–2, and CTC + I on the multibody plant.
- Added the result tables `results/NB08_*`.
- Closed-loop runs whose Cartesian error exceeds 5 mm are reported as lost and excluded from the summary means (see the README).

## v1.1.0 (2026-10-08)
- Added `NB06_fPINN_MemoriaFinita.ipynb` and `NB07_fPINN_HorizonteLargo.ipynb`: fixed-cost realizations of the learned Caputo memory term (window, window + tail correction, Oustaloup, sum of exponentials) in the CTC + I loop, 60 min operation and timing in Python and C.
- Added `c/NB07_fPINN_memkernels.c` (plain C99 kernels with benchmark) and the result tables `results/NB06_*` and `results/NB07_*`.
- Fixed the order of read and write in the ring buffer of the window + tail realization (NB06, Python and C). See the README.
- Added `threadpoolctl` to `requirements.txt`.

## v1.0.0 (2026-10-07)
- First release: NB01–NB05 and their result tables.
