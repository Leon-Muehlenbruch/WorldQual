Kurvenanpassung

Source:
G. Engeln-Müllges F.Reutter
Formelsammlung zur Numerischen Mathematik mit Turbo Pascal-Programmen
25 mat PO ENG

Nichlineare Approximation
F(x,c) = F(x, c0, c1, ..., cn) = c0* exp(c1 * x^2) + c2  // domestic Kurve
F(x,c) = F(x, c0, c1, ..., cn) = c0/(1+ exp(c1 * x+ c2)  // Wassertemperatur Kurve
F(x,c) = F(x, c0, c1, ..., cn) = c0/(1+ exp(c1 * x+ c2)  // Wassertemperatur Kurve mit 2 constanten (c0 - Höhe Vorgegeben)


Dieses nichtlineare Gleichungssystem wird mit dem gedämpften Newton-Verfahren gelöst.