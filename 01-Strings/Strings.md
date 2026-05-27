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

ToInteger: Σ* -> Z
ToInteger(s) = {
  (0, "si: " s = ε),
  ((valor(h) * 10^(GetLength(s) - 1)) + ToInteger(t), "si: " s = h.t)
}

IsPalindrome: Σ* -> B
IsPalindrome(s) = {
  (true, "si: " s = ε "o" GetLength(s) = 1),
  (IsPalindrome(t_medio), "si: " s = h.t_medio.u " y " h = u),
  (false, "en otro caso")
  }

Concatenate: Σ* × Σ* -> Σ*
Concatenate(s1, s2) = { 
  (s2, "si " s1 = ε), 
  (h1 . Concatenate(t1, s2), "si " s1 = h1.t1) 
  }

Power: Σ* × N -> Σ*
Power(s, n) = { 
  (ε, "si " n = 0), 
  (Concatenate(s, Power(s, n - 1)), "si " n > 0) 
  }

Reverse: Σ* -> Σ*
Reverse(s) = { 
  (ε, "si " s = ε), 
  (Concatenate(Reverse(t), h), "si " s = h.t) 
  }