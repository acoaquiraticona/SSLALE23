IsEmpty: (Sigma)* -> B
IsEmpty(s) = {(true, "Si longuitud(s) = epsilon"),(false, "En otro caso")}

GetLength: (Sigma)* -> N
GetLength(s) = {(0, s=epsilon),(1 + GetLength(t), h.t pertenecen a (Sigma))}

AreEqual: (Sigma)* -> (Sigma)* -> B
AreEqual(s,p) = { 
  (true, "si " s = epsilon " y " p = epsilon), 
  (AreEqual(t_s, t_p), "si " s = h_s.t_s, p = h_p.t_p " y " h_s = h_p), 
  (false, "en otro caso") 
}