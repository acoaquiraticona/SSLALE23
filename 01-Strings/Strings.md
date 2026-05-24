IsEmpty: (Sigma)* -> B
IsEmpty(s) = {(true, "Si longuitud(s) = epsilon"),(false, "En otro caso")}

GetLength: (Sigma)* -> N
GetLength(s) = {(0, s = epsilon),(1 + GetLength(t), h.t pertenecen a (Sigma))}

AreEqual: (Sigma)* -> (Sigma)* -> B
AreEqual(s,p) = { 
  (true, "si " s = epsilon " y " p = epsilon), 
  (AreEqual(t_s, t_p), "si " s = h_s.t_s, p = h_p.t_p " y " h_s = h_p), 
  (false, "en otro caso") 
}

AreDecimalDigits: Σ* -> B
AreDecimalDigits(s) = {
  (true, "si: " s = ε),
  (AreDecimalDigits(t), "si: " s = h.t " y " h pertenece a {0,1,2,3,4,5,6,7,8,9}),
  (false, "en otro caso")
}

Contains: Σ* × Σ -> B
Contains(s,c) = {
  (false, "si: " s = ε),
  (true, "si: " s = h.t " y " h = c ),
  (Contains(t,c), "si: " s = h.t " y " h ≠ c)
}