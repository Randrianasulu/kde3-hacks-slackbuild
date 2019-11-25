/*
    This file is part of libqobex.

    Copyright (c) 2003 Mathias Froehlich <Mathias.Froehlich@web.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef QBYTEBUFFER_H
#define QBYTEBUFFER_H

#include <qcstring.h>
#include <qvaluelist.h>

#include <string.h>

class QByteBuffer {
public:
  QByteBuffer() { mSize = 0; };

  void clear() {
/*     qDebug( "QByteBuffer(%X)::clear( .. )", this ); */
    QValueList<QByteArray>::Iterator it;
    for ( it = mBuffer.begin(); it != mBuffer.end(); )
      it = mBuffer.remove( it );
    mSize = 0;
  }
  
  Q_ULONG size() const {
/*     qDebug( "QByteBuffer(%X)::size()", this ); */
    QValueList<QByteArray>::ConstIterator it;
    Q_ULONG sz = 0;
    for ( it = mBuffer.begin(); it != mBuffer.end(); ++it )
      sz += (*it).size();
    Q_ASSERT( mSize == sz );
    return mSize;
  }
  bool isEmpty() const { return mSize <= 0; }

  char& at( Q_ULONG pos ) {
/*     qDebug( "QByteBuffer(%X)::at( %d ) mBuffer.size() = %d", int(pos), mBuffer.size() ); */
    QValueList<QByteArray>::Iterator it;
    for ( it = mBuffer.begin(); it != mBuffer.end(); ++it ) {
/*       qDebug( "QByteBuffer(%X)::at( ... ) pos = %d", int(pos) ); */
      if ( pos < (*it).size() )
	return (*it).at( pos );
      else
	pos -= (*it).size();
    }
    return *(char*)0;
  }
  const char& at( Q_ULONG pos ) const {
/*     qDebug( "QByteBuffer(%X)::at( %d ) const", int(pos) ); */
    QValueList<QByteArray>::ConstIterator it;
    for ( it = mBuffer.begin(); it != mBuffer.end(); ++it ) {
      if ( pos < (*it).size() )
	return (*it).at( pos );
      else
	pos -= (*it).size();
    }
    return *(const char*)0;
  }

  void put( const char *buf, Q_ULONG len ) {
/*     qDebug( "QByteBuffer(%X)::put( const char, size = %d )", this, len ); */
    if ( 0 < len ) {
      QByteArray tmp( len );
      ::memcpy( tmp.data(), buf, len );
      mBuffer.append( tmp );
      mSize += len;
    }
  }
  void put( QByteArray& tmp ) {
/*     qDebug( "QByteBuffer(%X)::put( QByteArray(size = %d) )", this, tmp.size() ); */
    if ( 0 < tmp.size() ) {
      mBuffer.append( tmp );
      mSize += tmp.size();
    }
  }
  int putch( int c ) {
/*     qDebug( "QByteBuffer(%X)::putch( .. )", this ); */
    QByteArray tmp( 1 );
    tmp[0] = c;
    mBuffer.append( tmp );
    ++mSize;
    return c;
  }

  Q_LONG get( char *buf, Q_ULONG len ) {
/*     qDebug( "QByteBuffer(%X)::get( .. )", this ); */
    Q_ULONG read = 0;
    QValueList<QByteArray>::Iterator it;
    for ( it = mBuffer.begin(); it != mBuffer.end(); ) {
      Q_ULONG todo = len - read;
      uint is = (*it).size();
      if ( is <= todo ) {
	::memcpy( buf+read, (*it).data(), is );
	read += is;
	it = mBuffer.remove( it );
      } else {
	::memcpy( buf+read, (*it).data(), todo );
	read += todo;
	qmemmove( (*it).data(), (*it).data()+todo, is-todo );
	(*it).resize( is-todo, QGArray::SpeedOptim );
	break;
      }
    }
    mSize -= read;
    return read;
  }

  int getch() {
/*     qDebug( "QByteBuffer(%X)::getch()", this ); */
    QValueList<QByteArray>::Iterator it = mBuffer.begin();
    if ( it == mBuffer.end() )
      return -1;
    else {
      uint sz = (*it).size();
      Q_ASSERT( 0 < sz );
      int c = (*it).at(0);
      if ( sz == 1 ) {
	mBuffer.remove( it );
      } else {
	qmemmove( (*it).data(), (*it).data()+1, sz );
	(*it).resize( sz-1, QGArray::SpeedOptim );
      }
      --mSize;
      return c;
    }
  }

  int ungetch( int c ) {
/*     qDebug( "QByteBuffer(%X)::ungetch()", this ); */
    QByteArray tmp( 1 );
    tmp[0] = c;
    mBuffer.prepend( tmp );
    ++mSize;
    return c;
  }
  
  Q_LONG readLine ( char * buf, Q_ULONG len ) {
/*     qDebug( "QByteBuffer(%X)::readLine()", this ); */
    if ( len <= 1 )
      return 0;
      
    len--;

    Q_ULONG read = 0;
    QValueList<QByteArray>::Iterator it;
    for ( it = mBuffer.begin(); it != mBuffer.end(); ) {
      Q_ULONG todo = len - read;
      uint is = (*it).size();
      int nlp = (*it).find( '\0' );
      if ( 0 <= nlp )
	todo = nlp + 1;
      if ( is <= todo ) {
	::memcpy( buf+read, (*it).data(), is );
	read += is;
	it = mBuffer.remove( it );
      } else {
	::memcpy( buf+read, (*it).data(), todo );
	read += todo;
	qmemmove( (*it).data(), (*it).data()+todo, is-todo );
	break;
      }
      if ( 0 <= nlp )
	break;
    }
    buf[read++] = '\0';
    mSize -= read;
    return read;
  }

  QString readLine() {
/*     qDebug( "QByteBuffer(%X)::readLine()", this ); */
    QString ret;

    int c;
    bool foundNewline = false;
    while ( 0 < ( c = getch() ) ) {
      if ( c != '\n' && c != '\r' ) {
	if ( foundNewline ) {
	  ungetch( c );
	  return ret;
	} else
	  ret += QChar( c );
      }
      if ( c == '\n' )
	foundNewline = true;
    }

    return ret;
  }

  bool contains( const char c ) const {
    QValueList<QByteArray>::ConstIterator it;
    for ( it = mBuffer.begin(); it != mBuffer.end(); ++it )
      if ( (*it).contains( c ) )
	return true;
    return false;
  }

private:
  Q_ULONG mSize;
  QValueList<QByteArray> mBuffer;
};

#endif
