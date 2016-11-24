# Alerter Calibration

### Prerequisites
- C++/ROOT

### Input data format

Tweets should exclude retweets.  At least one month of data is typically needed as rates are often low.

hour tweet_count

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


