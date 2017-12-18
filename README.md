# VdwNumberLow

A simple backtracker that searches for a Van der Waerden number lower bound.

If the program exhausts the search space then we can say that W(c, k) = lower bound+1.

Can complete the search for 5 values of W in a "reasonable" time on my computer (Windows 7 - Intel i3 3.5 GHz - Single core):

    W(2, 3) - instant
    W(2, 4) - instant
    W(2, 5) - 3m30s
    W(3, 3) - instant
    W(4, 3) - 18h

Avoids to test similar patterns, for example (0100 and 1011), and implements an explicit stack to handle high level of recursions.
