/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

\*---------------------------------------------------------------------------*/

#include "jjc2014Zone.H"
#include "fvMesh.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class Type>
void Foam::jjc2014Zone::modifyDdt(fvMatrix<Type>& m) const
{
    if (porosity_ < 1)
    {
        const labelList& cells = mesh_.cellZones()[cellZoneID_];

        scalar coeff( (1.0 + addedMassCoeff_ * ( 1 - porosity_ ) / porosity_) / porosity_ );

        forAll(cells, i)
        {
            m.diag()[cells[i]]   *= coeff;
            m.source()[cells[i]] *= coeff;
        }
    }
}

template<class RhoFieldType>
void Foam::jjc2014Zone::addViscousInertialResistance
(
    scalarField& Udiag,
    vectorField& Usource,
    const labelList& cells,
    const scalarField& V,
    const RhoFieldType& rho,
    const scalarField& mu,
    const vectorField& U
) const
{
    const tensor& D = D_.value();
    const tensor& F = F_.value();

    forAll (cells, i)
    {
//         tensor dragCoeff = mu[cells[i]]*D + (rho[cells[i]]*mag(U[cells[i]]))*F;
        
        const scalar magUxy = sqrt(U[cells[i]].x()*U[cells[i]].x() + U[cells[i]].y()*U[cells[i]].y());
        tensor dragCoeff = mu[cells[i]]*D + (rho[cells[i]]*magUxy)*F;
        scalar isoDragCoeff = tr(dragCoeff);

        Udiag[cells[i]] += V[cells[i]]*isoDragCoeff;
        Usource[cells[i]] -=
            V[cells[i]]*((dragCoeff - I*isoDragCoeff) & U[cells[i]]);
    }
}

template<class RhoFieldType>
void Foam::jjc2014Zone::addViscousInertialResistance
(
    tensorField& AU,
    const labelList& cells,
    const RhoFieldType& rho,
    const scalarField& mu,
    const vectorField& U
) const
{
    const tensor& D = D_.value();
    const tensor& F = F_.value();

    forAll (cells, i)
    {
//         AU[cells[i]] += mu[cells[i]]*D + (rho[cells[i]]*mag(U[cells[i]]))*F;
        const scalar magUxy = sqrt(U[cells[i]].x()*U[cells[i]].x() + U[cells[i]].y()*U[cells[i]].y());
        AU[cells[i]] += mu[cells[i]]*D + (rho[cells[i]]*magUxy)*F;
    }
}


// ************************************************************************* //
