;/////////////////////////////////////////////////////////////////////////////////////////////////////////////
;// Virtual Addr: xab00, Physical Addr: xcd00                                                               //
;// VPN = (xab >> 1) (7 bits)                                                                               //
;// System Table Address = PTBR + (VPN << 1) = x1000 + ((xab >> 1) << 1) = x10ab & 0x3FFE (last bit = 0)    //
;// Page Table Entry[Page Frame Number] = (xcd >> 1) (5 bits)                                               //
;// For System Space: PFN = VPN => "xcd[5:1] = xab[5:1]"                                                    //
;// PTE[15:8] = {0, 0, PFN, 0} = ((xab >> 1) << 1) = (xab & 0xFE)                                           //
;// ex: Vir Addr = x0200 => VPN = x01 => PFN = x01 => PTE[15:8] = x02 => MEM[PTE] = x0204 (Protect = 0)     //
;// System Space Page Range: x00 - x2F (PTE[15:8]: x00 - x2E)                                               //
;/////////////////////////////////////////////////////////////////////////////////////////////////////////////
.ORIG   x1000
; System Space Begin (x1000)
.FILL   x0004       
.FILL   x0204
.FILL   x0404
.FILL   x0604
.FILL   x0804
.FILL   x0A04
.FILL   x0C04
.FILL   x0E04
.FILL   x1004
.FILL   x1204
.FILL   x1404
.FILL   x1604
.FILL   x1804
.FILL   x1A04
.FILL   x1C04
.FILL   x1E04
.FILL   x2004
.FILL   x2204
.FILL   x2404
.FILL   x2604
.FILL   x2804
.FILL   x2A04
.FILL   x2C04
.FILL   x2E04
; System Space End  (x102E)
;/////////////////////////////////////////////////////////////////////////////////////////////////////////////
;// User Space Used: x30 -> x32(25), xC0 -> x38(28), xFC -> x3A(29)                                         //
;// SysTableAddr = x10ab & 0x3FFE: x30 -> x1030, xC0 -> x10C0, xFC -> x10FC                                 //
;// Page Frame Number(VPN): x32, x38, x3A                                                                   //
;// PTE[15:8]: x32, x38, x3A                                                                                //
;// Unused: P = 1(unprotected) => x0008, Used: V = 1(valid) => xab0C                                        //
;/////////////////////////////////////////////////////////////////////////////////////////////////////////////
; User Space Begin  (x1030)
.FILL   x320C       ;(x1030)
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008       ;(x1040)
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008       ;(x1050)
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008       ;(x1060)
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008       ;(x1070)
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008       ;(x1080)
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008       ;(x1090)
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008       ;(x10A0)
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008       ;(x10B0)
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x380C       ;(x10C0)
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008       ;(x10D0)
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008       ;(x10E0)
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008       ;(x10F0)
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x0008
.FILL   x3A0C       ;(x10FC)
.FILL   x0008
; User Space End
.END
