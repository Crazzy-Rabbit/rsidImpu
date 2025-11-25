# 📘rsidImpu

**A high-performance tool to annotate GWAS summary statistics with accurate rsIDs using dbSNP.**

`rsidImpu` is designed for large-scale genome-wide association studies (GWAS), providing fast and accurate rsID matching via **chromosome + position + allele comparison** (with support for allele flipping and strand complement).

The tool supports **QC filtering, SMR-formatted output, gzip input/output**, and **multi-thread acceleration**.

- **15GB+ dbSNP** reference files (tsv or gz)
- **millions of GWAS variants**
- **parallel processing (OpenMP)**
- **gzip input support**

---

## ✨ Key Features

### ✔ Accurate RSID Matching

- Matches variants by **CHR + POS + allele information**
- Supports:
  - Allele flipping (A1/A2 ↔ A2/A1)
  - Strand complement matching (A↔T, C↔G)

### ✔ High Performance

- OpenMP multithreading
- Efficient gzipped input/output
- Hash-based dbSNP lookup
- Capable of processing:
  - **15GB+ dbSNP files**
  - **Millions of GWAS variants**

### ✔ Clean & Flexible Output

- Matched rows → `<out>.txt`
- Unmatched rows → `<out>.txt.unmatched`
- GWAS alleles are **never modified** (A1/A2 remain as-is)
- Optional **SMR output format**: `SNP A1 A2 freq beta se P N`

### ✔ Built-in QC Module

(Optional, user-controlled)

- Remove invalid rows (non-finite N/beta/se/freq/P)
- Filter by MAF (default: 0.01)
- Remove duplicated SNPs (keep the one with lowest P)

## 📦 Installation

### **Compile with Makefile**

Requirements:

- C++11 or later
- zlib
- OpenMP (optional but recommended)

### Compile manually:

```
git clone https://github.com/Crazzy-Rabbit/rsidImpu.git
cd rsidImpu/src
make clean
make
```

The binary `rsidImpu` will be generated in the src/ directory.

## 🚀 Usage

### Basic example (default GWAS format)

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

### With QC enabled

```
rsidImpu \
  --gwas-summary example/gwas_test_clean.txt \
  --dbsnp example/dbsnp_test.txt \
  --out example/gwas_rsid.smr.gz \
  --dbchr CHR --dbpos POS --dbA1 REF --dbA2 ALT --dbrsid RSID \
  --format smr \
  --freq Freq --beta Beta --se SE --n N --pval P \
  --remove-dup-snp \
  --maf 0.01
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

Written to:  `<out>` or `<out>`.gz\

Format depends on --format:

- gwas → `original GWAS columns + SNP`
- smr → `SNP A1 A2 freq beta se P N`

### **Unmatched variants**

Written to: `<out>`.unmatched or `<out>`.unmatched.gz

### 🧪 Example Output (SMR Format)

```
SNP       A1  A2  freq   beta    se      P       N
rs1000    A   G   0.37   0.145   0.035   1e-5    50000
rs2000    T   C   0.42  -0.080   0.025   2e-3    50000
...
```

### 🧹 QC Module Summary

```
--remove-dup-snp	Remove duplicated SNPs (keep lowest P)
--maf <val>	        MAF threshold (default 0.01)
Auto-QC         	Remove lines with non-finite freq/beta/se/N/P, or p outside [0,1]
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
