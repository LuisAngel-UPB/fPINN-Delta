# fpinn-delta

Code and data for the article

> L. Angel, **Fractional physics-informed neural networks for the inverse dynamics of a delta parallel robot with memory-dependent joint damping**, submitted to *Nonlinear Dynamics* (2026).

A reduced 14-parameter inverse-dynamics model of a delta parallel robot is extended with a per-joint Caputo damper `τ_frac = B · D^α q`. A fractional physics-informed neural network (fPINN) predicts the parameters of the rigid-body regressor, while a fractional branch with learnable order `α` and gain `B` captures the memory torque. The L1 Caputo discretization is made differentiable in `α` by interpolation over a grid of 39 orders. The notebooks generate the data, train every model, and evaluate them open loop and in closed loop with computed-torque control plus integral action (CTC + I). Everything is simulated; no hardware is needed.

## Contents

```
notebooks/   NB01–NB05, run in this order (all outputs are written next to the notebooks)
results/     raw CSV outputs of the full runs used in the article
```

| Notebook | What it does | Main outputs |
|---|---|---|
| `NB01_fPINN_PlantaFraccionaria.ipynb` | Builds the fractional plant (14-parameter delta model + unmodelled torques `τ_u` + Caputo damper) and the random training trajectories. Identifies `α` and `B` by separable least squares (grid search over `α`) with 1 s and full memory, with and without `τ_u`. | `NB01_fPINN_FracLS_AllRuns.csv`, `fig_nb01_fpinn_alpha_profile.pdf` |
| `NB02_fPINN_EntrenamientoRedes.ipynb` | Trains the five networks (MLP, HYBRID, PINN and the fractional fHYBRID, fPINN) with 35 and 175 trajectories, seeds 0–2 for `α = 0.5`, and the order sweep `α ∈ {0.3, 0.7, 0.9}` (seed 0). Includes a check that the training code reproduces the original integer PINN. | `NB02_fPINN_TrainingLog.csv`, `NB02_fPINN_Summary.csv`, weights `NB02_fPINN_*_e150.npz`, cache `NB02_fPINN_cache/` |
| `NB03_fPINN_CTCLazoCerrado.ipynb` | Closed-loop validation: CTC + I on the fractional plant at 2 kHz with a 5 N m torque limit, Adept-like pick-and-place cycle at time scales 6.4, 3.2 and 1.6 with 0 and 6 kg payload, for the nominal model, a fractional oracle and every network of NB02. | `NB03_fPINN_ClosedLoop.csv`, `NB03_fPINN_Summary.csv` |
| `NB04_fPINN_RespuestaRevision.ipynb` | Completes the study: more seeds (0–4, and 0–9 for PINN and fPINN with 175 trajectories), three seeds for the order sweep, a null test on a plant without memory (`B = 0`), integer baselines with access to the motion history (`MLPhist`, `PINNmem`), an ablation without viscous friction (`b_v = 0`), encoder quantization, closed loop for all new networks, and paired statistics over seeds. Reuses the NB02 networks and NB03 runs. | `NB04_fPINN_TrainingLog.csv`, `NB04_fPINN_ClosedLoop.csv`, `NB04_fPINN_Summary.csv`, weights in `NB04_fPINN_weights/` |
| `NB05_fPINN_MemoriaPINNmem.ipynb` | Retrains nothing. Loads the NB04 `PINNmem` and `fPINN` weights and splits the torque of their memory branches into fractional, viscous and other components by least-squares projection. | `NB05_fPINN_MemoriaPINNmem.csv` |

The notebooks were developed after an earlier, unpublished set of baseline notebooks for the integer-order models; comments that mention "notebook 18" or "nb 18" refer to that baseline. Everything it provided (robot model, trajectory generator with the same random stream, training protocol and the CTC + I controller) is reproduced inside these notebooks, so it is not needed.

## Requirements

Python 3.9 with NumPy, SciPy, pandas, matplotlib, PyTorch (CPU build is enough) and psutil:

```bash
conda create -n fpinn-delta python=3.9
conda activate fpinn-delta
pip install -r requirements.txt
jupyter notebook
```

All runs reported in the article used PyTorch 2.8.0 (CPU) on a laptop with an AMD Ryzen 7 PRO 5850U (8 cores) and 16 GB of RAM under Windows 10. No GPU is used.

## How to run

Put the five notebooks in one folder (they are already together in `notebooks/`) and run them **in order**: each one reads files that the previous ones leave in the same folder.

NB01 has no options and runs in about 5 minutes. NB02–NB05 each have a single switch in their first code cell:

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

Total for the full study: about 13–22 h, best run overnight.

## Results in the article

The final tables and figures are computed from the full-run CSVs in `results/`:

| File | Rows | Used for |
|---|---|---|
| `NB01_fPINN_FracLS_AllRuns.csv` | 112 least-squares fits | least-squares identification of `α` (table and order-profile figure), LS rows of the accuracy tables |
| `NB04_fPINN_TrainingLog.csv` | 152 networks (NB02 rows included, `Scenario` column) | test and Adept-like torque errors, identified `α̂`, `B̂`, order sweep, null test, history baselines, ablations, paired comparisons |
| `NB04_fPINN_ClosedLoop.csv` | 825 closed-loop runs (NB03 rows included, quick-check rows removed) | closed-loop tracking table and figure, closed-loop paired comparisons and ablations |
| `NB05_fPINN_MemoriaPINNmem.csv` | 22 networks | decomposition of the `PINNmem` memory branch (history-baseline discussion) |

Network weights are not stored in the repository; they are regenerated by NB02 and NB04.

## Citation

If you use this code, please cite the article (and this software through the DOI shown on the Zenodo badge). See `CITATION.cff`.

## License

MIT, see `LICENSE`.
