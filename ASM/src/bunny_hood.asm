bunny_hood :
    ori     t0, r0, 0x04
    la      t1, GLOBAL_CONTEXT
    lw      t1, 0x1C44(t1)  ; Player in actor instance table
    beqz    t1, @@return
    nop

	la		t2, SAVE_CONTEXT
	lw		t3, 0x0004(t2)	; Link age
    lh      t4, 0x0066(t2)  ; Saved scene index (bunny hood dpad surrogate flag)
    nop

	beqz	t3, @@checkmask	; don't force equip if adult
    nop

    bne     t4, 0xF8, @@checkmask   ; don't force equip if dpad thing not set
    nop

    sb		t0, 0x14f(t1)	; Set worn mask to t0 = 4 = bunny hood
	nop

@@checkmask:
	nop
    lbu     t1, 0x14f(t1)   ; Worn Mask
    bne     t0, t1, @@return
    mtc1    t7, f4          ; Displaced
 
    lui     t0, 0x40B0
    bne     t0, a1, @@return
    nop

    lui     t0, 0x3fc0
    mtc1    t0, f22         ; f22 = 1.5
    nop
    mul.s   f12, f12, f22   ; f12 = 1.5 * horizontal velocity

@@return:
    nop
    mfc1    a1, f12; Displaced
    jr      ra
    nop