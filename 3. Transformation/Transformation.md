# Transformation

## Translate
$$
\begin{pmatrix}
1 & 0 & 0 & T<sub>x</sub> \\
0 & 1 & 0 & T<sub>y</sub> \\
0 & 0 & 1 & T<sub>z</sub> \\
0 & 0 & 0 & 1
\end{pmatrix}&middot;
\begin{pmatrix}
X \\
Y \\
Z \\
1
\end{pmatrix}=
\begin{pmatrix}
T<sub>x</sub> + x \\
T<sub>y</sub> + y \\
T<sub>z</sub> + z \\
1
\end{pmatrix}
$$

## Rotation
### Rotate X Axis
$$
\begin{pmatrix}
1 & 0 & 0 & 0 \\
0 & cos(&theta;) & -sin(&theta;) & 0 \\
0 & sin(&theta;) & cos(&theta;) & 0 \\
0 & 0 & 0 & 1
\end{pmatrix}&middot;
\begin{pmatrix}
x \\
y \\
z \\
1
\end{pmatrix}=
\begin{pmatrix}
x \\
cos(&theta;) &middot; y - sin(&theta;) &middot; z \\
sin(&theta;) &middot; y + cos(&theta;) &middot; z \\
1
\end{pmatrix}
$$

### Rotate Y Axis
$$
\begin{pmatrix}
cos(&theta;) & 0 & sin(&theta;) & 0 \\
0 & 1 & 0 & 0 \\
-sin(&theta;) & 0 & cos(&theta;) & 0 \\
0 & 0 & 0 & 1
\end{pmatrix}&middot;
\begin{pmatrix}
x \\
y \\
z \\
1
\end{pmatrix}=
\begin{pmatrix}
cos(&theta;) &middot; x + sin(&theta;) &middot; z \\
y \\
-sin(&theta;) &middot; x + cos(&theta;) &middot; z \\
1
\end{pmatrix}
$$

### Rotate Z Axis
$$
\begin{pmatrix}
cos(&theta;) & -sin(&theta;) & 0 & 0 \\
sin(&theta;) & cos(&theta;) & 0 & 0 \\
0 & 0 & 1 & 0 \\
0 & 0 & 0 & 1
\end{pmatrix}&middot;
\begin{pmatrix}
x \\
y \\
z \\
1
\end{pmatrix}=
\begin{pmatrix}
cos(&theta;) &middot x - sin(&theta;) &middot; z \\
sin(&theta;) &middot x + cos(&theta;) &middot; z \\
z \\
1
\end{pmatrix}
$$
### Combination X, Y, Z Axis
$$
\begin{pmatrix}
cos(&theta;) + R<sub>x</sub>^2(1 - cos(&theta;)) & R<sub>x</sub>R<sub>y</sub>(1 - cos(&theta;)) - R<sub>z</sub>sin(&theta;) & R<sub>x</sub>R<sub>z</sub>(1 - cos(&theta;)) + R<sub>y</sub>sin(&theta;) & 0 \\
R<sub>y</sub>R<sub>x</sub>(1 - cos(&theta;)) + R<sub>z</sub>sin(&theta;) & cos(&theta;) + R<sub>y</sub>^2(1 - cos(&theta;)) & R<sub>y</sub>R<sub>z</sub>(1 - cos(&theta;)) - R<sub>x</sub>sin(&theta;) & 0 \\
R<sub>z</sub>R<sub>x</sub>(1 - cos(&theta;)) - R<sub>y</sub>sin(&theta;) & R<sub>z</sub>R<sub>y</sub>(1 - cos(&theta;) + R<sub>x</sub>sin(&theta;) & cos(&theta;) + R<sub>z</sub>^2(1 - cos(&theta;)) & 0 \\
0 & 0 & 0 & 1
\end{pmatrix}
$$

## Scaling
$$
\begin{pmatrix}
scale & 0 & 0 & x \\
0 & scale & 0 & y \\
0 & 0 & scale & z \\
0 & 0 & 0 & 1
\end{pmatrix}
$$
