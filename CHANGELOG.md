# Changelog

## v1.1.0 (2026-10-08)
- Added `NB06_fPINN_MemoriaFinita.ipynb` and `NB07_fPINN_HorizonteLargo.ipynb`: fixed-cost realizations of the learned Caputo memory term (window, window + tail correction, Oustaloup, sum of exponentials) in the CTC + I loop, 60 min operation and timing in Python and C.
- Added `c/NB07_fPINN_memkernels.c` (plain C99 kernels with benchmark) and the result tables `results/NB06_*` and `results/NB07_*`.
- Fixed the order of read and write in the ring buffer of the window + tail realization (NB06, Python and C). See the README.
- Added `threadpoolctl` to `requirements.txt`.

## v1.0.0 (2026-10-07)
- First release: NB01–NB05 and their result tables.
