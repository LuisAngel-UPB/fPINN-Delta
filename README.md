# fpinn-delta

Code and data for the articles

> L. Angel, **Fractional physics-informed neural networks for the inverse dynamics of a delta parallel robot with memory-dependent joint damping**, submitted to *Nonlinear Dynamics* (2026). Notebooks NB01–NB05.

> L. Angel, **Finite-memory realizations of a learned fractional damper for real-time computed-torque control of a delta robot**, submitted (2026). Notebooks NB06–NB07 and the C kernels in `c/` (added in release v1.1.0).

A reduced 14-parameter inverse-dynamics model of a delta parallel robot is extended with a per-joint Caputo damper `τ_frac = B · D^α q`. A fractional physics-informed neural network (fPINN) predicts the parameters of the rigid-body regressor, while a fractional branch with learnable order `α` and gain `B` captures the memory torque. The L1 Caputo discretization is made differentiable in `α` by interpolation over a grid of 39 orders. The notebooks generate the data, train every model, and evaluate them open loop and in closed loop with computed-torque control plus integral action (CTC + I). Everything is simulated; no hardware is needed.

NB06–NB07 keep the identified networks fixed and study how the memory term `D^α q` of the controller can be evaluated at a fixed cost per step, so that the controller can run indefinitely: a short window, the window with an O(1) summation-by-parts tail correction, an Oustaloup filter in parallel modal form and a sum of exponentials (SOE) fitted to the Caputo kernel, compared with the full L1 sum in closed loop, over 60 min of operation with pauses, and in timing (Python and plain C99).

## Contents

```
notebooks/   NB01–NB07, run in this order (all outputs are written next to the notebooks)
c/           plain C99 kernels of the memory term (full L1, window + tail, Oustaloup, SOE) with a benchmark
results/     raw CSV outputs of the full runs used in the articles
```

| Notebook | What it does | Main outputs |
|---|---|---|
| `NB01_fPINN_PlantaFraccionaria.ipynb` | Builds the fractional plant (14-parameter delta model + unmodelled torques `τ_u` + Caputo damper) and the random training trajectories. Identifies `α` and `B` by separable least squares (grid search over `α`) with 1 s and full memory, with and without `τ_u`. | `NB01_fPINN_FracLS_AllRuns.csv`, `fig_nb01_fpinn_alpha_profile.pdf` |
| `NB02_fPINN_EntrenamientoRedes.ipynb` | Trains the five networks (MLP, HYBRID, PINN and the fractional fHYBRID, fPINN) with 35 and 175 trajectories, seeds 0–2 for `α = 0.5`, and the order sweep `α ∈ {0.3, 0.7, 0.9}` (seed 0). Includes a check that the training code reproduces the original integer PINN. | `NB02_fPINN_TrainingLog.csv`, `NB02_fPINN_Summary.csv`, weights `NB02_fPINN_*_e150.npz`, cache `NB02_fPINN_cache/` |
| `NB03_fPINN_CTCLazoCerrado.ipynb` | Closed-loop validation: CTC + I on the fractional plant at 2 kHz with a 5 N m torque limit, Adept-like pick-and-place cycle at time scales 6.4, 3.2 and 1.6 with 0 and 6 kg payload, for the nominal model, a fractional oracle and every network of NB02. | `NB03_fPINN_ClosedLoop.csv`, `NB03_fPINN_Summary.csv` |
| `NB04_fPINN_RespuestaRevision.ipynb` | Completes the study: more seeds (0–4, and 0–9 for PINN and fPINN with 175 trajectories), three seeds for the order sweep, a null test on a plant without memory (`B = 0`), integer baselines with access to the motion history (`MLPhist`, `PINNmem`), an ablation without viscous friction (`b_v = 0`), encoder quantization, closed loop for all new networks, and paired statistics over seeds. Reuses the NB02 networks and NB03 runs. | `NB04_fPINN_TrainingLog.csv`, `NB04_fPINN_ClosedLoop.csv`, `NB04_fPINN_Summary.csv`, weights in `NB04_fPINN_weights/` |
| `NB05_fPINN_MemoriaPINNmem.ipynb` | Retrains nothing. Loads the NB04 `PINNmem` and `fPINN` weights and splits the torque of their memory branches into fractional, viscous and other components by least-squares projection. | `NB05_fPINN_MemoriaPINNmem.csv` |
| `NB06_fPINN_MemoriaFinita.ipynb` | Keeps the networks fixed and replaces the full L1 sum of the controller's memory term by fixed-cost realizations (window, window + tail, Oustaloup with several orders and bands). Measures the offline error of the memory torque, the closed-loop tracking (CTC + I with the oracle and the fPINN and PINN of NB02/NB04 with 175 trajectories, α = 0.5 seeds 0–9 and α ∈ {0.3, 0.7, 0.9} seeds 0–2; 4 s and 8 s cycles and 60 s of continuous operation) and the cost per update in Python and C. | `NB06_fPINN_ClosedLoop.csv`, `NB06_fPINN_ErrorMemoria.csv`, `NB06_fPINN_Summary.csv`, `NB06_fPINN_Timing.csv`, `NB06_fPINN_TimingC.csv`, `NB06_fPINN_memkernels.c` |
| `NB07_fPINN_HorizonteLargo.ipynb` | E0: effect of the window + tail correction (see *Changes in v1.1.0*). E1: offline error over 60 min references with pauses at the ready pose, holds at a displaced pick pose and work away from the initial pose. E1b: hold limit of Oustaloup, `B·ω_b^α·|q̃|`, checked against the holds. E2: sum-of-exponentials (SOE) realization, kernel accuracy and closed loop as in NB06. E3: cost per update against operating time for the naive and block-FFT L1 and the fixed-cost realizations, in Python and in C. | `NB07_fPINN_CorreccionCola.csv`, `NB07_fPINN_LargoPorMinuto.csv`, `NB07_fPINN_LargoResumen.csv`, `NB07_fPINN_CotaOustaloup.csv`, `NB07_fPINN_SOE.csv`, `NB07_fPINN_ClosedLoop.csv`, `NB07_fPINN_Summary.csv`, `NB07_fPINN_Timing.csv`, `NB07_fPINN_TimingC.csv`, `NB07_fPINN_memkernels.c` |

The notebooks were developed after an earlier, unpublished set of baseline notebooks for the integer-order models; comments that mention "notebook 18" or "nb 18" refer to that baseline. Everything it provided (robot model, trajectory generator with the same random stream, training protocol and the CTC + I controller) is reproduced inside these notebooks, so it is not needed.

## Requirements

Python 3.9 with NumPy, SciPy, pandas, matplotlib, PyTorch (CPU build is enough), psutil and threadpoolctl (used by NB06–NB07 to time NumPy on one thread). The C timings of NB06–NB07 need a C compiler (`gcc`, `clang` or `cc`) on the PATH; without one that step is skipped and the `.c` file is still written:

```bash
conda create -n fpinn-delta python=3.9
conda activate fpinn-delta
pip install -r requirements.txt
jupyter notebook
```

All runs reported in the article used PyTorch 2.8.0 (CPU) on a laptop with an AMD Ryzen 7 PRO 5850U (8 cores) and 16 GB of RAM under Windows 10. No GPU is used.

## How to run

Put the five notebooks in one folder (they are already together in `notebooks/`) and run them **in order**: each one reads files that the previous ones leave in the same folder.

NB06 and NB07 need the NB02 weights (`NB02_fPINN_*_e150.npz`) and the folder `NB04_fPINN_weights` in the same folder; in quick mode without them they use a stand-in model and say so.

NB01 has no options and runs in about 5 minutes. NB02–NB07 each have a single switch in their first code cell:

- `QUICK = True` (default): fast check of the whole pipeline with few epochs and runs, in about 2 minutes per notebook. Its outputs are tagged (`_e2`, `_e3`, `_QUICK`) so they never mix with the full run, and NB03–NB05 in quick mode read the quick networks of the previous notebooks.
- `QUICK = False`: runs the complete experiment set of the article, with no other edits.

Full runs are resumable: every finished network is cached and every closed-loop run is appended to the CSV, so after an interruption simply rerun the notebook.

| Notebook | `QUICK = True` | `QUICK = False` (reference laptop) | RAM |
|---|---|---|---|
| NB01 | (no switch) | ~5 min | < 2 GB |
| NB02 | ~2 min | 8–14 h (42 networks) | ~2 GB |
| NB03 | ~2 min | 15–30 min (300 runs) | < 1 GB |
| NB04 | ~2 min | 4–7 h (110 new networks, ~530 closed-loop runs) | ~3 GB |
| NB05 | ~15 s | ~1 min | < 1 GB |
| NB06 | ~2 min | 40 min – 1.5 h (about 1 000 closed-loop runs) | < 2 GB |
| NB07 | ~2 min | 50 min – 1 h 40 min (60 min references, ~600 closed-loop runs) | < 3 GB |

Total for NB01–NB05: about 13–22 h, best run overnight; NB06–NB07 add about 2–3 h.

## Results in the articles

The final tables and figures are computed from the full-run CSVs in `results/`:

| File | Rows | Used for |
|---|---|---|
| `NB01_fPINN_FracLS_AllRuns.csv` | 112 least-squares fits | least-squares identification of `α` (table and order-profile figure), LS rows of the accuracy tables |
| `NB04_fPINN_TrainingLog.csv` | 152 networks (NB02 rows included, `Scenario` column) | test and Adept-like torque errors, identified `α̂`, `B̂`, order sweep, null test, history baselines, ablations, paired comparisons |
| `NB04_fPINN_ClosedLoop.csv` | 825 closed-loop runs (NB03 rows included, quick-check rows removed) | closed-loop tracking table and figure, closed-loop paired comparisons and ablations |
| `NB05_fPINN_MemoriaPINNmem.csv` | 22 networks | decomposition of the `PINNmem` memory branch (history-baseline discussion) |
| `NB06_fPINN_ErrorMemoria.csv` | 324 | offline error of the memory torque on the task references, Oustaloup and window rows (the `+ tail` rows come from the first NB06 run, see *Changes in v1.1.0*) |
| `NB06_fPINN_ClosedLoop.csv`, `NB06_fPINN_Summary.csv` | 1 023 runs, 336 rows | closed loop of the full L1 sum, no memory term, plain windows and Oustaloup; gain kept over the integer PINN (`+ tail` rows from the first NB06 run, superseded by NB07) |
| `NB06_fPINN_Timing.csv` | 65 | Python cost per update and RAM against operating time |
| `NB07_fPINN_CorreccionCola.csv` | 72 | offline error of the window + tail before and after the correction |
| `NB07_fPINN_ClosedLoop.csv`, `NB07_fPINN_Summary.csv` | 609 runs, 192 rows | closed loop of the corrected window + tail and of the SOE (with the L1 and Oustaloup reference rows) |
| `NB07_fPINN_LargoPorMinuto.csv`, `NB07_fPINN_LargoResumen.csv` | 15 616, 256 | 60 min offline study, per minute and per run |
| `NB07_fPINN_CotaOustaloup.csv` | 52 | hold limit of Oustaloup against the error measured at the end of the holds |
| `NB07_fPINN_SOE.csv` | 40 | number of modes, cost and kernel accuracy of the SOE and of Oustaloup read as a sum of exponentials |
| `NB07_fPINN_Timing.csv` | 66 | Python cost per update (mean, median and worst step) against operating time, including the block-FFT L1 |
| `NB07_fPINN_TimingC.csv` | 15 | C cost per update (gcc -O2, x86-64 server processor) |

Network weights are not stored in the repository; they are regenerated by NB02 and NB04.

## C kernels

`c/NB07_fPINN_memkernels.c` (also written by NB07) contains the update functions of the full L1 sum, the window + tail, the Oustaloup filter (parallel Tustin modes) and the SOE (nodes for α = 0.5 included), for three joints in double precision, plus a benchmark `main` that checks the four kernels on a test input and prints the cost per update as CSV. The update functions use no dynamic memory, so they can be moved to an embedded controller.

```bash
gcc -O2 -std=gnu99 c/NB07_fPINN_memkernels.c -o memk -lm && ./memk > timing.csv
```

## Changes in v1.1.0

- Added NB06, NB07, the C kernels and their result tables (second article).
- Fixed the window + tail realization: the first NB06 run wrote the newest sample into the ring buffer before reading the discarded sample stored in the same slot, so the mean correction averaged the recent past instead of the discarded history. The NB06 of this release reads the slot first (Python `MemWindow.push` and C `win_update`). NB07 keeps the old code only to measure the difference (E0); all window + tail results of the second article come from NB07. Plain windows, Oustaloup, SOE and the L1 sum were not affected.

## Citation

If you use this code, please cite the article (and this software through the DOI shown on the Zenodo badge). See `CITATION.cff`.

## License

MIT, see `LICENSE`.
