#include "BCH_Reed_Solomon.h"

#include <cassert>
#include <cstring>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <memory>

//==============================================================================

static const std::vector<uint32_t> gf2_4_exp2vect = 
{
	1,2,4,8,3,6,12,11,5,10,7,14,15,13,9,0
};

static const std::vector<uint32_t> gf2_4_vect2exp = 
{
	15,0,1,4,2,8,5,10,3,14,9,7,6,13,11,12
};

static const std::vector<uint32_t> gf2_8_exp2vect =
{
	1,2,4,8,16,32,64,128,29,58,116,232,205,135,19,38,
	76,152,45,90,180,117,234,201,143,3,6,12,24,48,96,192,
	157,39,78,156,37,74,148,53,106,212,181,119,238,193,159,35,
	70,140,5,10,20,40,80,160,93,186,105,210,185,111,222,161,
	95,190,97,194,153,47,94,188,101,202,137,15,30,60,120,240,
	253,231,211,187,107,214,177,127,254,225,223,163,91,182,113,226,
	217,175,67,134,17,34,68,136,13,26,52,104,208,189,103,206,
	129,31,62,124,248,237,199,147,59,118,236,197,151,51,102,204,
	133,23,46,92,184,109,218,169,79,158,33,66,132,21,42,84,
	168,77,154,41,82,164,85,170,73,146,57,114,228,213,183,115,
	230,209,191,99,198,145,63,126,252,229,215,179,123,246,241,255,
	227,219,171,75,150,49,98,196,149,55,110,220,165,87,174,65,
	130,25,50,100,200,141,7,14,28,56,112,224,221,167,83,166,
	81,162,89,178,121,242,249,239,195,155,43,86,172,69,138,9,
	18,36,72,144,61,122,244,245,247,243,251,235,203,139,11,22,
	44,88,176,125,250,233,207,131,27,54,108,216,173,71,142,0
};

static const std::vector<uint32_t> gf2_8_vect2exp =
{
	255,0,1,25,2,50,26,198,3,223,51,238,27,104,199,75,
	4,100,224,14,52,141,239,129,28,193,105,248,200,8,76,113,
	5,138,101,47,225,36,15,33,53,147,142,218,240,18,130,69,
	29,181,194,125,106,39,249,185,201,154,9,120,77,228,114,166,
	6,191,139,98,102,221,48,253,226,152,37,179,16,145,34,136,
	54,208,148,206,143,150,219,189,241,210,19,92,131,56,70,64,
	30,66,182,163,195,72,126,110,107,58,40,84,250,133,186,61,
	202,94,155,159,10,21,121,43,78,212,229,172,115,243,167,87,
	7,112,192,247,140,128,99,13,103,74,222,237,49,197,254,24,
	227,165,153,119,38,184,180,124,17,68,146,217,35,32,137,46,
	55,63,209,91,149,188,207,205,144,135,151,178,220,252,190,97,
	242,86,211,171,20,42,93,158,132,60,57,83,71,109,65,162,
	31,45,67,216,183,123,164,118,196,23,73,236,127,12,111,246,
	108,161,59,82,41,157,85,170,251,96,134,177,187,204,62,90,
	203,89,95,176,156,169,160,81,11,245,22,235,122,117,44,215,
	79,174,213,233,230,231,173,232,116,214,244,234,168,80,88,175
};

//==============================================================================

BCHReedSolomon::Nomial::Nomial(void *gf,uint32_t x)
: val(x)
, m_gf(gf)
{
	((BCHReedSolomon::Field*)(this->m_gf))->pool[x]=this;
}

BCHReedSolomon::Nomial *BCHReedSolomon::Nomial::instance(void *gf,uint32_t x)
{
	x = x % (((BCHReedSolomon::Field*)gf)->pool.size());
	if (!((BCHReedSolomon::Field*)gf)->pool[x])
		new Nomial(gf, x);

	return (((BCHReedSolomon::Field*)gf)->pool[x]);
}

BCHReedSolomon::Nomial *BCHReedSolomon::Nomial::dup()
{
	return (BCHReedSolomon::Nomial::instance(this->m_gf, this->val));
}


uint32_t BCHReedSolomon::Nomial::to_exp()
{
	return (this->val);
}

uint32_t BCHReedSolomon::Nomial::to_vect()
{
	return (((BCHReedSolomon::Field*)this->m_gf)->exp2vect[this->val]);
}

inline int BCHReedSolomon::Nomial::m()
{
	return (((BCHReedSolomon::Field*)(this->m_gf))->m);
}

inline int BCHReedSolomon::Nomial::n()
{
	return (((BCHReedSolomon::Field*)(this->m_gf))->n);
}

inline uint32_t BCHReedSolomon::Nomial::exp2vect(uint32_t x)
{
	return (((BCHReedSolomon::Field*)(this->m_gf))->exp2vect[x]);
}

inline uint32_t BCHReedSolomon::Nomial::vect2exp(uint32_t x)
{
	return (((BCHReedSolomon::Field*)(this->m_gf))->vect2exp[x]);
}

bool BCHReedSolomon::Nomial::is_zero()
{
	return (this->to_vect() == 0);
}

bool BCHReedSolomon::Nomial::operator==(BCHReedSolomon::Nomial x)
{
	return (this->val == x.val);
}

bool BCHReedSolomon::Nomial::operator!=(BCHReedSolomon::Nomial x)
{
	return (this->val != x.val);
}

BCHReedSolomon::Nomial BCHReedSolomon::Nomial::operator+(BCHReedSolomon::Nomial x)
{
	return (*BCHReedSolomon::Nomial::instance(this->m_gf, this->vect2exp(this->to_vect() ^ x.to_vect())));
}

BCHReedSolomon::Nomial BCHReedSolomon::Nomial::operator-(BCHReedSolomon::Nomial x)
{
	return (*this + x);
}

BCHReedSolomon::Nomial BCHReedSolomon::Nomial::operator*(BCHReedSolomon::Nomial x)
{
	if (this->is_zero() || x.is_zero())
		return (*(((BCHReedSolomon::Field*)(this->m_gf))->zero()));
	else return (*BCHReedSolomon::Nomial::instance(this->m_gf, (this->val+x.val)%this->n()));
}

BCHReedSolomon::Nomial BCHReedSolomon::Nomial::operator/(BCHReedSolomon::Nomial x)
{
	if (x.is_zero())
		throw std::runtime_error("x nomial is zero");
	else if (this->is_zero())
		return (*(((BCHReedSolomon::Field*)(this->m_gf))->zero()));
	else return (*BCHReedSolomon::Nomial::instance(this->m_gf, (this->val + this->n()-x.val) % this->n()));
}

//==============================================================================

BCHReedSolomon::Field::Field(int _m)
: m(_m)
{
	int i;
	for(i = 0, this->n = 1; i < m; i++)
		this->n *= 2;
	this->n--;

	switch(this->m)
	{
		case 4:
			this->exp2vect = gf2_4_exp2vect.data();
			this->vect2exp = gf2_4_vect2exp.data();
			break;
		case 8:
			this->exp2vect = gf2_8_exp2vect.data();
			this->vect2exp = gf2_8_vect2exp.data();
			break;
		default:
			throw std::invalid_argument("m is invalid");
	}

   this->pool.resize(n + 1);
   for (i = 0; i < this->pool.size(); i++)
   {
	   this->pool[i] = nullptr;
	   BCHReedSolomon::Nomial::instance(this, i);
   }
}

BCHReedSolomon::Field::~Field()
{
	for (size_t i = 0; i< this->pool.size(); i++)
	{
		if (this->pool[i])
			delete this->pool[i];
	}
}

BCHReedSolomon::Nomial* BCHReedSolomon::Field::exp2nomial(uint32_t x)
{
	return (this->pool[x % (this->n)]);
}

BCHReedSolomon::Nomial* BCHReedSolomon::Field::vect2nomial(uint32_t x)
{
	return (this->pool[this->vect2exp[x % (this->n)]]);
}

BCHReedSolomon::Nomial* BCHReedSolomon::Field::zero()
{
	return (this->pool[this->n]);
}

//==============================================================================

BCHReedSolomon::Polynomial::Polynomial()
: cols(0)
, rows(0)
{}

BCHReedSolomon::Polynomial::Polynomial(int _rows)
: cols(1)
, rows(_rows)
, nomial(rows)
{}

BCHReedSolomon::Polynomial::Polynomial(int _cols, int _rows)
: cols(_cols)
, rows(_rows)
, nomial(cols * rows)
{}

BCHReedSolomon::Polynomial::~Polynomial()
{}

BCHReedSolomon::Polynomial *BCHReedSolomon::Polynomial::dup()
{
	BCHReedSolomon::Polynomial* ret = new BCHReedSolomon::Polynomial(this->cols, this->rows);
	ret->nomial = this->nomial;

	return ret;
}

BCHReedSolomon::Polynomial *BCHReedSolomon::Polynomial::dup(int count)
{
	return this->dup(0, 0, count, count);
}

BCHReedSolomon::Polynomial *BCHReedSolomon::Polynomial::dup(int start_col,int start_row,int count)
{
	return this->dup(start_col, start_row, count, count);
}

BCHReedSolomon::Polynomial *BCHReedSolomon::Polynomial::dup(int start_col,int start_row,
	int col_count,int row_count)
{
	BCHReedSolomon::Polynomial* ret = new BCHReedSolomon::Polynomial(col_count, row_count);

	for(int i = 0, c = start_col; i < col_count; i++, c++)
	{
		for (int j = 0, r = start_row; j < row_count; j++, r++)
			ret->set(i, j, this->get(c, r));
	}

	return ret;
}


BCHReedSolomon::Nomial *BCHReedSolomon::Polynomial::set(int row, BCHReedSolomon::Nomial *val)
{
	return this->set(0, row, val);
}

BCHReedSolomon::Nomial *BCHReedSolomon::Polynomial::set(int col, int row, BCHReedSolomon::Nomial *val)
{
	this->nomial[col * rows + row] = val->dup();
	return this->nomial[col * rows + row];
}

BCHReedSolomon::Nomial *BCHReedSolomon::Polynomial::get(int row)
{
	return this->get(0, row);
}

BCHReedSolomon::Nomial *BCHReedSolomon::Polynomial::get(int col, int row)
{
	return this->nomial[col * rows + row]->dup();
}


BCHReedSolomon::Polynomial *BCHReedSolomon::Polynomial::lu()
{
	BCHReedSolomon::Polynomial* buf = this->dup();
	BCHReedSolomon::Polynomial* ret = buf->_lu(buf);
	if(!ret)
		delete buf;

	return ret;
}

BCHReedSolomon::Polynomial *BCHReedSolomon::Polynomial::lu(int count)
{
	BCHReedSolomon::Polynomial *buf=this->dup(0,0,count);
	BCHReedSolomon::Polynomial *ret=buf->_lu(buf);
	if(!ret)
		delete buf;
	return(ret);
}

BCHReedSolomon::Polynomial *BCHReedSolomon::Polynomial::lu(int start_col,int start_row,int count)
{
	BCHReedSolomon::Polynomial *buf=this->dup(start_col,start_row,count);
	BCHReedSolomon::Polynomial *ret=buf->_lu(buf);
	if(!ret)
		delete buf;
	return(ret);
}

BCHReedSolomon::Polynomial *BCHReedSolomon::Polynomial::_lu(BCHReedSolomon::Polynomial *buf)
{
	int count = buf->cols, i, j;
	if (buf->rows < buf->cols)
		count=buf->rows;

	for(j = 0; j < count; j++)
	{
		Nomial* l;
		for (i = j; i < count; i++)
		{
			l = buf->get(i, j);
			if (!l->is_zero())
				break;
		}
		if (i >= count)
			return(nullptr);
		else if(i > j)
			buf->swap_col(j, i);

		for(i=j+1;i<count;i++)
		{
			Nomial n= (*buf->get(i, j) / (*l));
			buf->set(i, j, &n);
			for(int k = j + 1; k < count; k++)
			{
				Nomial m = (*buf->get(i, k) - *buf->get(j, k) * n);
				*buf->set(i, k, &m);
			}
		}
	}

	return buf;
}

BCHReedSolomon::Polynomial *BCHReedSolomon::Polynomial::solve()
{
	BCHReedSolomon::Polynomial* lu = this->lu();
	if (lu)
	{
		BCHReedSolomon::Polynomial* ret=this->solve(lu);
		delete lu;
		return ret;
	}

	return nullptr;
}

BCHReedSolomon::Polynomial *BCHReedSolomon::Polynomial::solve(BCHReedSolomon::Polynomial *lu)
{
	if (!lu)
		return nullptr;

	if (lu->rows != this->cols + 1)
		throw std::runtime_error("lu->rows != this->cols + 1");

	int rank = 0, i, j, k;
	for (j = 0; j < lu->cols; j++)
	{
		bool is_zero_cols = true;
		for (k = j; k < lu->cols; k++)
			is_zero_cols &= lu->get(j, k)->is_zero();

		if(!is_zero_cols)
			rank++;
	}
	if (rank < lu->cols)
		return nullptr;

	BCHReedSolomon::Polynomial *ret = new BCHReedSolomon::Polynomial(lu->cols);
	for (i = 0; i < lu->cols; i++)
		ret->set(i, lu->get(i, lu->cols));

	for(j = 0; j < lu->cols; j++)
	{
		for (i = j + 1; i < lu->cols; i++)
		{
			auto n = (*ret->get(i) - *ret->get(j) * *lu->get(i, j));
			ret->set(i, &n);
		}
	}
	for (j = lu->cols - 1; j >= 0; j--)
	{
		for (int i = j + 1; i < lu->cols; i++)
		{
			auto n = (*ret->get(j)-*lu->get(j, i) * *ret->get(i));
			ret->set(j, &n);
		}
		auto n = (*ret->get(j) / (*lu->get(j, j)));
		ret->set(j, &n);
	}

	return ret;
}

void BCHReedSolomon::Polynomial::swap_col(int i,int j)
{
	size_t sz = sizeof(BCHReedSolomon::Nomial*) * this->rows;

	std::vector<BCHReedSolomon::Nomial*> tmp(this->rows);

	memcpy(tmp.data(), this->nomial.data() + i * this->rows, sz);
	memcpy(this->nomial.data() + i * this->rows, this->nomial.data() + j * this->rows, sz);
	memcpy(this->nomial.data() + j * this->rows, tmp.data(), sz);
}

//==============================================================================

BCHReedSolomon::BCH::BCH(BCHReedSolomon::Field *gf, int size, int capability)
: Polynomial::Polynomial(size)
, m_gf(gf)
, m_capability(capability)
{}

BCHReedSolomon::BCH::~BCH()
{}

int BCHReedSolomon::BCH::decode(int syndrome_base)
{
	this->syndromes.resize(this->m_capability * 2 + syndrome_base);

	int errors = 0, i;
	for (i = 0; i < this->syndromes.size(); i++)
	{
		this->syndromes[i] = this->_error_syndrome(i);
		if (!this->syndromes[i]->is_zero())
			errors++;
	}
	if (!errors)
		return 0;

	BCHReedSolomon::Polynomial *err = nullptr;
	for (errors = this->m_capability; errors > 0; errors--)
	{
		BCHReedSolomon::Polynomial mat(errors, errors + 1);

		for (int j = 0; j < errors; j++)
		{
			for(i = 0; i <= errors;i++)
				mat.set(j, i, this->syndromes[i + j + syndrome_base]);
		}
		err = mat.solve();

		if(err)
			break;
	}
	if (!err)
		return -1;

	if (err)
	{
		this->error_pos.resize(errors);
		std::memset(this->error_pos.data(), -1, this->error_pos.size());

		int c, i, j;
		for(j = 0, c = 0; j < this->rows; j++)
		{
			BCHReedSolomon::Nomial sigma = *err->get(0);
			for(i = 1; i < errors; i++)
				sigma = (sigma + *err->get(i) * *this->m_gf->exp2nomial(j * i));
			sigma = (sigma + *this->m_gf->exp2nomial(j * i));

			if(sigma.is_zero())
			{
				if (c < errors)
					this->error_pos[c] = j;

				c++;
			}
		}
		delete err;

		if (c == errors)
			return errors;
	}

	return -1;
}

BCHReedSolomon::Nomial *BCHReedSolomon::BCH::_error_syndrome(int d)
{
	BCHReedSolomon::Nomial x = *this->m_gf->zero();

	for(int i=0;i<this->rows;i++)
		x = (x + *this->get(i) * *this->m_gf->exp2nomial(i * d));

	return x.dup();
}

//==============================================================================

BCHReedSolomon::BCHReedSolomon(uint32_t dataBytes, uint32_t eccBytes)
: BCHReedSolomon::BCHReedSolomon(8, 0x11d, 0, 1, eccBytes, 255 - dataBytes - eccBytes)
{
	m_totalBytes = dataBytes + eccBytes;
}

BCHReedSolomon::BCHReedSolomon(int symSize, int gfpoly, int fcr, int prim, int nroots, int pad)
: m_mm(symSize)
, m_nn((1 << symSize) - 1)
, m_pad(pad)
, m_fcr(fcr)
, m_prim(prim)
, m_nroots(nroots)
, m_gfpoly(gfpoly)
{
	if (((m_mm < 0 || m_mm > 8))
		|| (m_fcr < 0 || m_fcr >= (1 << m_mm))
		|| (m_prim <= 0 || m_prim >= (1 << m_mm))
		|| (m_nroots < 0 || m_nroots >= (1 << m_mm))
		|| (m_pad < 0 || m_pad >= (m_nn - m_nroots))
	)
		throw std::invalid_argument("Invalid arguments");
	
	m_alpha_to.reset(new uint32_t[m_nn + 1]);
	m_index_of.reset(new uint32_t[m_nn + 1]);

	int sr = 1, i;
	m_index_of.get()[0] = m_nn;
	m_alpha_to.get()[m_nn] = 0;
	for (i = 0; i < m_nn; i++)
	{
		m_index_of.get()[sr] = i;
		m_alpha_to.get()[i] = sr;
		sr <<= 1;
		if (sr & (1 << m_mm))
			sr ^= m_gfpoly;
		sr &= m_nn;
	}
	if (sr != 1)
		throw std::logic_error("Field generator polynomial is not primitive");
	
	int iprim, root;
	m_genpoly.reset(new uint32_t[m_nroots + 1]);
	for (iprim = 1; (iprim % m_prim) != 0; iprim += m_nn);
	m_iprim = iprim / m_prim;

	m_genpoly.get()[0] = 1;
	for (i = 0, root = m_fcr * m_prim; i < m_nroots; i++, root += m_prim)
	{
		m_genpoly.get()[i + 1] = 1;
		for (int j = i; j > 0; j--)
		{
			if (m_genpoly.get()[j] != 0)
				m_genpoly.get()[j] = m_genpoly.get()[j - 1] ^ m_alpha_to.get()[ModNN(m_index_of.get()[m_genpoly.get()[j]] + root)];
			else m_genpoly.get()[j] = m_genpoly.get()[j - 1];
		}
		
		m_genpoly.get()[0] = m_alpha_to.get()[ModNN(m_index_of.get()[m_genpoly.get()[0]] + root)];
	}
	for (i = 0; i < m_nroots; i++)
		m_genpoly.get()[i] = m_index_of.get()[m_genpoly.get()[i]];
}

int BCHReedSolomon::ModNN(int x)
{
	while (x >= m_nn)
	{
		x -= m_nn;
		x = (x >> m_mm) + (x & m_nn);
	}
	return x;
}

void BCHReedSolomon::Encode(const void* _data, size_t dataSize, void* ecc, size_t eccSize)
{
	assert(_data != nullptr);
	assert(ecc != nullptr);
	
	if (dataSize != m_totalBytes - m_nroots || eccSize != m_nroots)
		throw std::invalid_argument("Sizes are wrong");
	
	const uint8_t* data = (const uint8_t*)_data;
	uint8_t* parity = (uint8_t*)ecc;
	
	std::memset(parity, 0, m_nroots);
	for (int i = 0; i < m_nn - m_nroots - m_pad; i++)
	{
		uint8_t feedback = m_index_of.get()[data[i] ^ parity[0]];
		if (feedback != m_nn)
		{
			for (int j = 1; j < m_nroots; j++)
				parity[j] ^= m_alpha_to.get()[ModNN(feedback + m_genpoly.get()[m_nroots - j])];
		}

		std::memmove(&parity[0], &parity[1], m_nroots - 1);
		if (feedback != m_nn)
			parity[m_nroots - 1] = m_alpha_to.get()[ModNN(feedback + m_genpoly.get()[0])];
		else parity[m_nroots - 1] = 0;
	}
}

int BCHReedSolomon::Decode(void* _data, size_t dataSize)
{
	assert(_data != nullptr);

	if (dataSize != m_totalBytes)
		throw std::invalid_argument("Data's size is wrong");
	
	uint8_t* data = (uint8_t*)_data;
	
	Field gf(8);
	BCH bch(&gf, m_totalBytes, m_nroots / 2);
	
	int i, j;
	for (i = m_totalBytes - 1, j = 0; i >= 0; i--, j++)
		bch.set(j, gf.vect2nomial(data[i]));
	
	int errors = bch.decode();
	if (errors > 0)
	{
		std::unique_ptr<Polynomial> mat(new Polynomial(errors, errors + 1));
		for (j = 0; j < errors; j++)
		{
			for(i = 0; i < errors; i++)
				mat->set(j, i, gf.exp2nomial(j * bch.error_pos[i]));

			mat->set(j, i, bch.syndromes[j]);
		}
		std::unique_ptr<Polynomial> err(mat->solve());
		
		if (err)
		{
			for(i = 0; i < errors; i++)
			{
				int p = m_totalBytes - bch.error_pos[i] - 1;
				data[p] = (*gf.vect2nomial(data[p]) + *err->get(i)).to_vect();
			}
		}
	}
	
	return errors;
}

int BCHReedSolomon::DecodeQRFormatData(short& data)
{
	BCHReedSolomon::Field gf(4);
	BCHReedSolomon::BCH bch(&gf, 15, 3);
	int16_t mask = 1;
	int i;
	for(i = 0; i < 15; i++, mask <<= 1)
	{
		if (data & mask)
			bch.set(i, gf.exp2nomial(0));
		else bch.set(i, gf.zero());
	}

	int errors = bch.decode();
	if (errors > 0)
	{
		mask = 1;
		for (i = 0; i < errors; i++)
			data ^= (mask << bch.error_pos[i]);
	}

	return errors;
}
