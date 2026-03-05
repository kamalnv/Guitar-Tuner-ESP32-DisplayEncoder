# PCB Order Specifications for Indian Manufacturers

## Board Specifications

| Parameter | Value |
|-----------|-------|
| **Board Size** | 100mm x 50mm |
| **Layers** | 2 (Top + Bottom) |
| **Thickness** | 1.6mm |
| **Copper Weight** | 1 oz (35µm) |
| **Surface Finish** | HASL (Lead-free) |
| **Solder Mask** | Green (both sides) |
| **Silkscreen** | White (top side) |
| **Min Track Width** | 0.25mm (10 mil) |
| **Min Spacing** | 0.25mm (10 mil) |
| **Min Hole Size** | 0.8mm |
| **Material** | FR-4 |

## Files Included

```
gerber/
├── esp32_tuner-F_Cu.gbr      (Top Copper)
├── esp32_tuner-Edge_Cuts.gbr (Board Outline)
├── esp32_tuner-F_SilkS.gbr   (Top Silkscreen)
├── esp32_tuner-F_Mask.gbr    (Top Solder Mask)
└── esp32_tuner.drl           (Drill File - Excellon)
```

## Drill Sizes

| Size | Count | Purpose |
|------|-------|---------|
| 0.8mm | 28 | Resistors, capacitors, IC |
| 1.0mm | 25 | Pin headers |
| 1.2mm | 6 | Wire pads |
| 3.2mm | 4 | M3 mounting holes |

## Indian PCB Manufacturers - How to Order

### Option 1: LionCircuits (Recommended)
1. Go to: https://www.lioncircuits.com
2. Click "Get Quote" or "Order PCB"
3. Upload the `gerber/` folder as a ZIP file
4. Select specifications as above
5. Get instant quote

### Option 2: PCB Power
1. Go to: https://www.pcbpower.com
2. Click "Instant Quote"
3. Upload Gerber ZIP
4. Configure options
5. Get quote

### Option 3: Twisted Traces
1. Go to: https://www.twistedtraces.com
2. Upload files
3. Get quote

## Estimated Pricing (2026)

| Quantity | LionCircuits | PCB Power | Notes |
|----------|--------------|-----------|-------|
| 5 pcs | ₹800-1,200 | ₹700-1,000 | Prototype |
| 10 pcs | ₹1,000-1,500 | ₹900-1,300 | Small batch |
| 25 pcs | ₹2,000-3,000 | ₹1,800-2,500 | |
| 50 pcs | ₹3,500-5,000 | ₹3,000-4,500 | Volume |

*Prices are estimates, exclude GST (18%) and shipping*

## Lead Time

| Manufacturer | Standard | Express |
|--------------|----------|---------|
| LionCircuits | 5-7 days | 3 days |
| PCB Power | 7-10 days | 4-5 days |

## How to Create Gerber ZIP

```bash
cd gerber/
zip esp32_tuner_gerber.zip *.gbr *.drl
```

Then upload `esp32_tuner_gerber.zip` to the manufacturer's website.

## Assembly Service (Optional)

Both LionCircuits and PCB Power offer PCBA (assembly) services:
- Provide BOM (Bill of Materials) 
- They source and solder components
- Adds ₹200-500 per board for simple through-hole assembly

## Contact Information

**LionCircuits**
- Website: lioncircuits.com
- Email: support@lioncircuits.com
- Location: Bangalore

**PCB Power**
- Website: pcbpower.com
- Email: info@pcbpower.com
- Location: Gujarat

## Notes for Ordering

1. Select "Through-hole" components (no SMD)
2. Specify "Panelization: No" for small quantities
3. Choose "E-testing: Yes" for quality check
4. Standard 1.6mm thickness fits 125B enclosure
