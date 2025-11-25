# rsidImpu

**A high-performance tool to annotate GWAS summary statistics with correct rsid using dbSNP.**

`rsidImpu` is a lightweight, fast, and scalable C++ tool designed for genome-wide association study (GWAS) datasets.
It matches GWAS variants to dbSNP records using **chromosome + position + allele matching (with flip & strand complement support)** and outputs both matched and unmatched variants.

This tool is optimized for performance and can handle extremely large datasets, including:

- **15GB+ dbSNP** reference files (tsv or gz)
- **millions of GWAS variants**
- **parallel processing (OpenMP)**
- **gzip input support**

---

## ✨ Features

### ✔ Accurate RSID Matching

- Matches variants by **chromosome + position + allele information**
- Allows:
  - Allele flipping (A1/A2 ↔ A2/A1)
  - Strand complement matching (A↔T, C↔G)

### ✔ High Performance

- Multithreaded (OpenMP)
- Streamed reading of large files
- Zero-copy gzip reader
- Hash-based dbSNP lookup (`unordered_map`)

### ✔ Clean Output

- Matched rows → `<out>.txt`
- Unmatched rows → `<out>.txt.unmatched`
- GWAS alleles are **never modified** (A1/A2 remain as-is)
- SMR output format available: `SNP A1 A2 freq beta se P N`

## 📦 Installation

### **Compile with Makefile**

Requirements:

- C++11 or later
- zlib
- OpenMP (optional but recommended)

If needed:
```
sudo apt install zlib1g-dev
sudo apt install libomp-dev
```
### Compile manually:

```
git clone https://github.com/Crazzy-Rabbit/rsidImpu.git
cd rsidImpu/src
make
```

## 🚀 Usage
### Basic example (default)

```
rsidImpu
  --gwas-summary example/gwas_test_clean.txt \
  --dbsnp example/dbsnp_test.txt \
  --out example/gwas_rsid.txt \
  --dbchr CHR --dbpos POS --dbA1 REF --dbA2 ALT --dbrsid RSID
```

### SMR Output Format Example
```
rsidImpu \
  --gwas-summary example/gwas_test_clean.txt \
  --dbsnp example/dbsnp_test.txt \
  --out example/gwas_rsid.smr.gz \
  --dbchr CHR --dbpos POS --dbA1 REF --dbA2 ALT --dbrsid RSID \
  --format smr \
  --freq Freq --beta Beta --se SE --n N --pval P
```

### Show help

```
rsidImpu --help
```

---

## 📥 Input Formats

### **GWAS summary statistics**

Required columns:

- CHR (or custom name)
- POS
- A1
- A2
- P (p-value)

Additional columns needed for SMR format:

- freq
- beta
- se
- N

### **dbSNP reference**

Two supported formats:

#### 1️⃣ TSV/CSV with header

Required columns:

- CHR
- POS
- REF
- ALT
- RSID

#### 2️⃣ PLINK `.bim` or `.bim.gz`
Automatically parsed as:
```
CHR  SNP  CM  POS  A1  A2
```

## 📤 Output

### **Matched variants**

Written to:  <out> or <out>.gz\

Format depends on --format:

- gwas → `original GWAS columns + SNP`
- smr → `SNP A1 A2 freq beta se P N`

### **Unmatched variants**

Written to: <out>.unmatched or <out>.unmatched.gz

### 🧪 Example Output (SMR Format)
```
SNP       A1  A2  freq   beta    se      P       N
rs1000    A   G   0.37   0.145   0.035   1e-5    50000
rs2000    T   C   0.42  -0.080   0.025   2e-3    50000
...
```
### 🔧 Notes

- Allele matching allows:
  - swap/flip: A1/A2 ↔ A2/A1
  - strand complement: A↔T, C↔G
- Input alleles are **never modified**
- Only matched rows are included in the main output
- Gzip input/output is supported automatically based on filename suffix .gz

## ❤️ Acknowledgement
Special thanks to ChatGPT for code assistance and architectural optimization during tool development.

## 📫Contract
If you have any questions or suggestions, feel free to reach out:

📧 crazzy_rabbit@163.com
