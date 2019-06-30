# encode2ssat
Code that transforms POMDP and Probabilistic Inference problems into SSAT problems.

compile the encoders with:
> make pomdp network

To run binaries generated:
./network path-to-bayes-network-file.UAI > encoded-network.SSAT
./pomdp path-to-pomdp-file.POMDP horizon-length > encoded-pomdp.SSAT

