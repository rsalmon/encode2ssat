# encode2ssat
Code that transforms POMDP and Probabilistic Inference problems into SSAT problems.

compile the encoders with:
> make pomdp network

To run binaries generated:
./network path-to-bayes-network-file.UAI > encoded-network.SSAT
./pomdp path-to-pomdp-file.POMDP horizon-length > encoded-pomdp.SSAT

POMDP problems: https://drive.google.com/open?id=1U6nXhjzFPuNpiuxf2vmylxn8CsryEc-a
Bayesian Inference problems: https://drive.google.com/open?id=1d1CoXBT52B-84d851mdr8Z6bB216JrCA

