# Alerter Calibration

### Prerequisites
- C++/ROOT

### Input data format

Tweets should exclude retweets.  At least one month of data is typically needed as rates are often low.  Format of data files expected is multiple lines with hour of day and tweet count in 10 minute intervals:

```
hour tweet_count
```

The data is first prepared for fitting (after relevant paths set) using:

```
root bin_gfdrr.cxx
```


### Usage

Thresholds can then be generated using:

```
root fit_gfdrr.cxx
```

### Caveats

Tweet distributions are assumed to be Poisson within the time bins, so for example, retweets must be excluded.  Fits should always be checked for convergence in the graphs and text output of the fitting code.  If fits are stuggling to converge check there is enough input data and that sensible starting parameters are being set.

